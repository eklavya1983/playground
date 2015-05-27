#include <exception>
#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <util/Log.h>
#include <actor/gen-cpp2/ConfigApi.h>
#include <actor/ActorSystem.hpp>
#include <actor/ActorUtil.h>
#include <actor/RemoteActor.h>

DEFINE_int32(actorthreads, 2, "Number of actor threads");
DEFINE_int32(serverthreads, 2, "Number of server io threads");

namespace actor {

ActorSystem::ActorSystem()
    : ActorSystem(invalidActorId(), "test", nullptr, 0, "0", 0)
{
}

ActorSystem::ActorSystem(const ActorSystemId &id)
    : ActorSystem({apache::thrift::FRAGILE, id, LOCALID_START},
                  "test", nullptr, 0, "0", 0)
{
}

ActorSystem::ActorSystem(const ActorId &id,
                         const std::string &systemType,
                         std::unique_ptr<ServiceApiSvIf> handler,
                         int myPort,
                         const std::string &configIp,
                         int configPort)
    : NotificationQueueActor(this),
    // TODO: Revisit siziing this. Two optins
    // 1. Make it unordered_map
    // 2. Have config service determine the size
    actorTbl_(1024)
{
    /* Set up threadpools so work can be done */
    ioThreadPool_ = std::make_shared<folly::wangle::IOThreadPoolExecutor>(FLAGS_actorthreads);
    /* Set my event base immediately */
    auto eb = getNextEventBase();
    setEventBase(eb);

    configIp_ = configIp;
    configPort_ = configPort;
    
    systemInfo_.type = systemType;
    // TODO: Detect your ip here
    systemInfo_.ip = "127.0.0.1";
    systemInfo_.port = myPort;
    // TODO: Set this on restarts
    systemInfo_.incarnation = 1;

    /* Do registration if required */
    if (id == invalidActorId()) {
        systemInfo_.id = invalidActorId();
        registerWithConfigService_();
    } else {
        /* No need to register.  We will assign an id */
        myId_ = id;
        systemInfo_.id = myId_;
    }
    DCHECK(myId_ != invalidActorId() &&
           systemInfo_.id != invalidActorId() &&
           myId_.localId == LOCALID_START) ;
    DCHECK(systemInfo_.id == myId_);

    nextLocalActorId_ = LOCALID_START + 1;

    /* Start the server */
    if (!handler) {
        handler.reset(new ServiceHandler(this));
    }
    server_.reset(new ReplicaActorServer(this, std::move(handler),
                                         FLAGS_serverthreads, myPort)); 

    ALog(INFO) << "ActorSystem constructed";
}

ActorSystem::~ActorSystem()
{
    server_->stop();
    ALog(INFO) << "ActorSystem destructed";
}

void ActorSystem::init() {
    CHECK(this == system_);

    /* We need to add ourself in to actorTbl so messages can be routed */
    actorTbl_.insert(std::make_pair(toInt64(myId_), getPtr()));

    NotificationQueueActor::init();
}

void ActorSystem::registerWithConfigService_() {

    using namespace apache::thrift;
    using namespace apache::thrift::async;
    using namespace apache::thrift::transport;

    TEventBase base;
    std::shared_ptr<TAsyncSocket> socket(
        TAsyncSocket::newSocket(&base, configIp_, configPort_));
    auto client_channel = HeaderClientChannel::newChannel(socket);
    std::unique_ptr<ConfigApiAsyncClient> client_(new ConfigApiAsyncClient(std::move(client_channel)));

    try {
        client_->sync_registerActorSystem(myId_, systemInfo_);
    } catch (std::exception &e) {
        CHECK(false) << "Exception raised in registration.  " << e.what();
        throw;
    }
    systemInfo_.id = myId_;
}

void ActorSystem::initBehaviors_()
{
    NotificationQueueActor::initBehaviors_();
    initBehavior_ += {
        on(Init) >> [this]() {
            #if 0
            initConfigRemoteActor_();
            sendRegisterMsg_();
            #endif
        }
        #if 0
        on(RegisterResp) >> [this]() {
            auto &respPayload = payload<RegisterResp>();
            setId(respPayload.id);
            systemInfo_.id = respPayload.id;
            ALog(INFO) << "Registration complete: " << systemInfo_;
            changeBehavior(&functionalBehavior_);
        }
        #endif
    };
    functionalBehavior_ += {
        on(GetActorRegistry) >> [this]() {
            /// reply(msg, getActorRegistry());  
        }
    };
}

ActorPtr ActorSystem::createRemoteActor_(const ActorInfo &info) {
    DCHECK(getEventBase()->isInEventBaseThread());
    /* For now we only create remote actor/proxies for actors systems */
    CHECK(isActorSystemId(info.id));
    CHECK(lookUpActor(info.id) == nullptr);
    
    ALog(INFO) << "Creating remote actor: " << info;

    ActorPtr actor = std::make_shared<RemoteActor>(this, getNextEventBase());
    actor->setId(info.id);
    actorTbl_.insert(std::make_pair(toInt64(info.id), actor));
    actor->init();

    /* We don't send init message to remote actor.  Instead we send
     * UpdateActorInfo as the init method
     * See updateActorRegistry_() it also doen the same thing when
     * creating remote actor
     */
    auto payload = std::make_shared<UpdateActorInfo>();
    payload->info = info;
    ACTOR_SEND(actor,
        makeActorMsg<UpdateActorInfo>(myId(), info.id, std::move(payload)));

    return actor;
}

Error ActorSystem::updateActorRegistry_(const ActorInfo &info, bool createActor) {
    DCHECK(getEventBase()->isInEventBaseThread());
    /* For now we only expect updateActorRegistry_() for actor systems that are remote */
    CHECK(isActorSystemId(info.id));

    /* Update registry with incoming actor registry info */
    auto entry = actorRegistry_.find(toInt64(info.id));
    if (entry == actorRegistry_.end()) {
        actorRegistry_[toInt64(info.id)] = info;
    } else {
        if (info.incarnation < entry->second.incarnation) {
            AVLog(LCONFIG) << "Failed to update registry.  Incarnation # is less.  Current: "
                << entry->second.incarnation << " input: " << info.incarnation;
            return Error::ERR_INVALID;
        }
        entry->second = info;
    }
    AVLog(LCONFIG) << "Updating actor registry with new info: " << info;

    /* Create actor(if requested) or update existing actor with new information */
    auto actor = lookUpActor(info.id);
    if (actor) {
        /* For existing remote actor send a message to update connection */
        auto payload = std::make_shared<UpdateActorInfo>();
        payload->info = info;
        ACTOR_SEND(actor,
            makeActorMsg<UpdateActorInfo>(myId(), info.id, std::move(payload)));
    } else if (createActor) {
        /* Create new remote actor if requested */
        createRemoteActor_(info);
    }
    return Error::ERR_OK;
}

void ActorSystem::sendRegisterMsg_() {
#if 0
    ALog(INFO) << "Sending registration message";

    auto payload = std::make_shared<Register>();
    payload->systemInfo = systemInfo_;
    auto msg = makeActorMsg<Register>(myId(), configActorId(), payload);
    // TODO: Check how routeToActor() is working here
    ROUTE(std::move(msg));
#endif
}

void ActorSystem::initConfigRemoteActor_()
{
    createRemoteActor_(ActorInfo(apache::thrift::FRAGILE,
                                 "config",
                                 configActorId(),
                                 configIp_,
                                 configPort_,
                                 0));
}

ActorPtr ActorSystem::lookUpActor(const ActorId &id)
{
    auto itr = actorTbl_.find(toInt64(id));
    if (itr == actorTbl_.end()) {
        return nullptr;
    }
    return itr->second;
}

void ActorSystem::loop() {
    server_->start(true);
}

void ActorSystem::handleInitMsg(ActorMsg &&msg) {
    assert(!"TODO: Implement");
}

void ActorSystem::handleUpdateActorTableMsg(ActorMsg &&msg) {
    assert(!"TODO: Implement");
}

std::shared_ptr<folly::wangle::IOThreadPoolExecutor> ActorSystem::getIOThreadPool()
{
    return ioThreadPool_;
}

folly::EventBase* ActorSystem::getNextEventBase()
{
    return ioThreadPool_->getEventBase();
}

}  // namespace actor

#include <actor/ActorSystem.hpp>
#include <actor/ActorUtil.h>
#include <actor/RemoteActor.h>
#include <util/Log.h>

namespace actor {

ActorSystem::ActorSystem(const std::string &systemType,
                         int myPort,
                         const std::string &configIp,
                         int configPort)
    : NotificationQueueActor(nullptr)
{
    systemInfo_.type = systemType;
    systemInfo_.id = invalidActorId();
    // TODO: Detect your ip here
    systemInfo_.ip = "127.0.0.1";
    systemInfo_.port = myPort;
    // TODO: Set this on restarts
    systemInfo_.incarnation = 1;
    configIp_ = configIp_;
    configPort_ = configPort;
    nextLocalActorId_ = LOCALID_START;

    // TODO: Revisit setting event base here
    setEventBase(&eventBase_);
    server_.reset(new ReplicaActorServer(this, myPort)); 
}

ActorSystem::~ActorSystem()
{
    server_->stop();
    eventBase_.terminateLoopSoon();
}

void ActorSystem::init() {

    // TODO: start the server
    server_->start();
    NotificationQueueActor::init();
}

void ActorSystem::initBehaviors_()
{
    NotificationQueueActor::initBehaviors_();
    initBehavior_ += {
        on(Init) >> [this]() {
            initConfigRemoteActor_();
            sendRegisterMsg_();
        },
        on(RegisterResp) >> [this]() {
            auto &payload = msgPayload<RegisterResp>();
            setId(payload.id);
            systemInfo_.id = payload.id;
            ALog(INFO) << "Registration complete: " << systemInfo_;
            changeBhavior(&functionalBehavior_);
        }
    };
    functionalBehavior_ += {
        on(GetActorRegistry) >> [this]() {
            /// reply(msg, getActorRegistry());  
        }
    };
}

ActorPtr ActorSystem::createRemoteActor_(const ActorInfo &info) {
    DCHECK(getEventBase()->isInEventBaseThread());
    // CHECK(info.id.systemId == ROOT_ACTOR_LOCAL_ID);
    CHECK(lookUpActor(info.id) == nullptr);
    
    ALog(INFO) << info;

    ActorPtr actor = std::make_shared<RemoteActor>(this, &eventBase_, info);
    actor->setId(info.id);
    actorTbl_->insert(std::make_pair(toInt64(info.id), actor));

    auto payload = std::make_shared<UpdateActorInfo>();
    payload->info = info;
    actor->send(
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
            return Error::ERR_INVALID;
        }
        entry->second = info;
    }

    auto actor = lookUpActor(info.id);
    if (actor) {
        /* For existing remote actor send a message to update connection */
        auto payload = std::make_shared<UpdateActorInfo>();
        payload->info = info;
        actor->send(
            makeActorMsg<UpdateActorInfo>(myId(), info.id, std::move(payload)));
    } else if (createActor) {
        /* Create new remote actor if requested */
        createRemoteActor_(info);
    }
    return Error::ERR_OK;
}

void ActorSystem::sendRegisterMsg_() {
    ALog(INFO) << "Sending registration message";

    auto payload = std::make_shared<Register>();
    payload->systemInfo = systemInfo_;
    auto msg = makeActorMsg<Register>(myId(), configActorId(), payload);
    // TODO: Check how routeToActor() is working here
    routeToActor(std::move(msg));
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
    auto itr = actorTbl_->find(toInt64(id));
    if (itr == actorTbl_->end()) {
        return nullptr;
    }
    return itr->second;
}

void ActorSystem::loop() {
    eventBase_.loopForever();
}

void ActorSystem::handleInitMsg(ActorMsg &&msg) {
    assert(!"TODO: Implement");
}

void ActorSystem::handleUpdateActorTableMsg(ActorMsg &&msg) {
    assert(!"TODO: Implement");
}

}  // namespace actor

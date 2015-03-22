#include <actor/ActorSystem.h>
#include <actor/ActorUtil.h>
#include <actor/RemoteActor.h>
#include <util/Log.h>

namespace actor {

ActorSystem::ActorSystem(int myPort,
                         const std::string &configIp,
                         int configPort)
    : NotificationQueueActor(nullptr)
{
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
    NotificationQueueActor::init();

    server_->start();
    // TODO: send init message
}

void ActorSystem::initBehaviors_()
{
    NotificationQueueActor::initBehaviors_();
    initBehavior_ += {
        on(Init) >> [this](ActorMsg &&msg) {
            initConfigRemoteActor_();
            sendRegisterMsg_();
        },
        on(RegisterActorSystemResp) >> [this](ActorMsg &&msg) {
            changeBhavior(&functionalBehavior_);
        }
    };
    functionalBehavior_ += {
        on(GetActorRegistry) >> [this](ActorMsg &&msg) {
            /// reply(msg, getActorRegistry());  
        }
    };
#if 0
    ACTORMSGHANDLER(initBehavior_,
                    ActorMsgTypeIds::InitMsg,
                    ActorSystem::handleInitMsg)
    ACTORMSGHANDLER(initBehavior_,
                    ActorMsgTypeIds::RegistrationRespMsg,
                    ActorSystem::handleRegistrationRespMsg)
    
    ACTORMSGHANDLER(functionalBehavior_,
                    ActorMsgTypeIds::UpdateActorTableMsg,
                    ActorSystem::handleUpdateActorTableMsg)
#endif

}

void ActorSystem::createRemoteActor_(const ActorInfo &info) {
    DCHECK(getEventBase()->isInEventBaseThread());
    CHECK(info.id.systemId == ROOT_ACTOR_LOCAL_ID);
    CHECK(lookUpActor(info.id) == nullptr);
    
    ALog(INFO) << info;

    ActorPtr a = std::make_shared<RemoteActor>(this, &eventBase_, info);
    a->setId(info.id);
    actorTbl_->insert(std::make_pair(toInt64(info.id), a));
}

void ActorSystem::updateActorRegistry_(const ActorInfo &info, bool createActor) {
    DCHECK(getEventBase()->isInEventBaseThread());
    actorRegistry_[toInt64(info.id)] = info;

    auto actor = lookUpActor(info.id);
    if (createActor && !actor) {
        createRemoteActor_(info);
    }
    if (actor) {
        auto payload = std::make_shared<UpdateActorInfo>();
        payload->info = info;
        actor->send(
            makeActorMsg<UpdateActorInfo>(id_, info.id, std::move(payload)));
    }
}

void ActorSystem::sendRegisterMsg_()
{
    CHECK(false) << "Not implemented";
}

void ActorSystem::initConfigRemoteActor_()
{
    CHECK(false) << "Not implemented";
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

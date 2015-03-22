#include <actor/ActorSystem.h>
#include <actor/ActorUtil.h>

namespace actor {

ActorSystem::ActorSystem(int myPort,
                         const std::string &configIp,
                         int configPort)
    : NotificationQueueActor()
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

    initSerializers_();

    server_->start();

    register_();
}

void ActorSystem::initSerializers_()
{
    MAP_SERAILIZER(ActorMsgTypes::GetActorRegistryMsg, GetActorRegistry);
}

void ActorSystem::initBehaviors_()
{
    NotificationQueueActor::initBehaviors_();
    functionalBehavior_ += {
        on(ActorMsgTypes::GetActorRegistryMsg) >> [this](ActorMsg &&msg) {
            /// reply(msg, getActorRegistry());  
        }
    };
#if 0
    ACTORMSGHANDLER(initBehavior_,
                    ActorMsgTypes::InitMsg,
                    ActorSystem::handleInitMsg)
    ACTORMSGHANDLER(initBehavior_,
                    ConfigMsgTypes::RegistrationRespMsg,
                    ActorSystem::handleRegistrationRespMsg)
    
    ACTORMSGHANDLER(functionalBehavior_,
                    ActorMsgTypes::UpdateActorTableMsg,
                    ActorSystem::handleUpdateActorTableMsg)
#endif

}

void ActorSystem::register_()
{
    // TODO
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

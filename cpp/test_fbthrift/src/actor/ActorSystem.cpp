#include <ActorSystem.h>

namespace actor {

ActorSystem::ActorSystem(int myPort,
                         const std::string &configIp,
                         int configPort)
{
    server_.reset(new ReplicaActorServer(myPort)); 
}

ActorSystem::~ActorSystem()
{
    server_->stop();
}

void ActorSystem::init()
{
    server_->start();
    register_();
}

void ActorSystem::register_()
{
    // TODO
}

void ActorSystem::registerActor(const ActorId &id, const ActorPtr &actorRef)
{
    actorTbl_->insert(std::make_pair(id, actorRef));
}

ActorPtr ActorSystem::lookUpActor(const ActorId &id)
{
    auto itr = actorTbl_->find(id);
    if (itr == actorTbl_->end()) {
        return nullptr;
    }
    return itr->second;
}

}  // namespace actor

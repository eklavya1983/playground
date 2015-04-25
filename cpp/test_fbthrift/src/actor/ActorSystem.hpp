#include <actor/ActorSystem.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

using namespace cpp2;

template<class ActorT, class ... ArgsT>
ActorPtr ActorSystem::spawnRootActor(ArgsT&&... args) {
    CHECK(nextLocalActorId_ == LOCALID_START);
    CHECK(actorTbl_->size() == 0);
    return spawnActor<ActorT>(std::forward<ArgsT>(args)...);
}

template<class ActorT, class ... ArgsT>
ActorPtr ActorSystem::spawnActor(ArgsT&&... args)
{
    ActorId id;

    ActorPtr a = std::make_shared<ActorT>(this, &eventBase_, std::forward<ArgsT>(args)...);
    id.systemId = myId().systemId;
    id.localId = nextLocalActorId_++;
    a->setId(id);
    actorTbl_->insert(std::make_pair(toInt64(id), a));
    a->send(makeActorMsg<Init>());

    return a;
}

template <class MsgT>
bool ActorSystem::routeToActor(MsgT&& msg)
{
    auto actor = lookUpActor(msg.to());
    if (!actor) {
        return false;
    }
    actor->send(std::forward<MsgT>(msg));
    return true;
}

}

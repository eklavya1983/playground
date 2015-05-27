#include <actor/ActorSystem.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

using namespace cpp2;

#if 0
template<class ActorT, class ... ArgsT>
ActorPtr ActorSystem::spawnActor(ArgsT&&... args)
{
    auto eb = system_->getEventBase();
    CHECK(eb != nullptr);
    ActorPtr a = std::make_shared<ActorT>(this,
                                          getNextEventBase(),
                                          std::forward<ArgsT>(args)...);
    auto future = via(eb).then([this, &a, eb]() {
        DCHECK(eb->isInEventBaseThread());
        ActorId id;
        id.systemId = myId().systemId;
        id.localId = nextLocalActorId_++;
        a->setId(id);
        actorTbl_.insert(std::make_pair(toInt64(id), a));
        a->init();
    });
    future.wait();
    return a;
}
#endif
template<class ActorT, class ... ArgsT>
std::shared_ptr<ActorT> ActorSystem::spawnActor(ArgsT&&... args)
{
    std::shared_ptr<ActorT> a = std::make_shared<ActorT>(this,
                                          getNextEventBase(),
                                          std::forward<ArgsT>(args)...);
    ActorId id;
    id.systemId = myId().systemId;
    id.localId = nextLocalActorId_++;
    a->setId(id);
    actorTbl_.insert(std::make_pair(toInt64(id), a));
    a->init();
    return a;
}

template <class MsgT>
bool ActorSystem::routeToActor(MsgT&& msg)
{
    ActorPtr actor;

    if (isRemoteId(msg.to())) {
        actor = lookUpActor(toRemoteId(msg.to()));
    } else {
        actor = lookUpActor(msg.to());
    }

    if (!actor) {
        /* During registration actor system won't have an id until registration complets.
         * Route RegisterResp directly to actor system
         */
         #if 0
        if (myId_ == invalidActorId() &&
            msg.typeId() == ActorMsgTypeEnum<RegisterResp>::typeId) {
            ACTOR_SEND(getPtr(), std::forward<MsgT>(msg));
            return true;
        }
        #endif
        CHECK(false) << "Failed to route message: " << msg;
        return false;
    }

    ACTOR_SEND(actor, std::forward<MsgT>(msg));
    return true;
}

}

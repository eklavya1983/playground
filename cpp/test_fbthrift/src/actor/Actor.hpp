#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>

namespace actor {
template <class MsgT>
void Actor::reply(Payload &&payload) {
    auto replyMsg = makeActorMsg<MsgT>(myId_, from(),  std::move(payload)); 
    system_->routeToActor(std::move(replyMsg));
}
}  // namespace actor

#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>

namespace actor {
template <class MsgT>
void Actor::reply(const ActorMsg& msg, Payload &&payload) {
    auto replyMsg = makeActorMsg<MsgT>(id_, msg.first.from, payload); 
    system_->routeToActor(std::move(replyMsg));
}
}  // namespace actor

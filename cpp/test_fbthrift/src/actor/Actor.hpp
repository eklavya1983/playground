#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>

namespace actor {
template <class MsgTEnum>
void Actor::reply(const ActorMsg& msg, const MsgTEnum &type, Payload &&payload) {
    auto replyMsg = makeActorMsg(type, id_, msg.first.from, payload); 
    system_->routeToActor(std::move(replyMsg));
}
}  // namespace actor

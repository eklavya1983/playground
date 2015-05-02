#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>
#include <actor/ActorRequest.h>

namespace actor {
template <class MsgT>
void Actor::reply(Payload &&payload) {
    /* Current message for which we are replying must be normal direction */
    DCHECK(direction() != MSGDIRECTION_NORMAL);
    auto replyMsg = makeActorMsgCommon<MsgT>(MSGDIRECTION_RESPONSE,
                                             myId_,
                                             from(),
                                             requestId(),
                                             std::move(payload)); 
    system_->routeToActor(std::move(replyMsg));
}
}  // namespace actor

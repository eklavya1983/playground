#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>
#include <actor/ActorRequest.h>

namespace actor {

template <class PayloadRetT>
std::shared_ptr<PayloadRetT> Actor::sendSync(Payload &&payload) {
    auto& req = system_->getTracker()->allocRequest<ActorRequest>();
    auto future = req.toActor(myId())
        .withPayload(payload)
        .fire();
    future.wait();
    auto respMsg = std::move(future.value());
    return respMsg->buffer<PayloadRetT>();
}

template <class MsgT>
void Actor::reply(Payload &&payload) {
    /* Current message for which we are replying must be normal direction */
    DCHECK(direction() != MSGDIRECTION_NORMAL);
    auto replyMsg = makeActorMsgCommon<MsgT>(MSGDIRECTION_RESPONSE,
                                             myId_,
                                             from(),
                                             requestId(),
                                             std::move(payload)); 
    ROUTE(std::move(replyMsg));
}
}  // namespace actor

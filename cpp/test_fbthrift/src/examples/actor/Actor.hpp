#pragma once
#include <actor/Actor.h>
#include <actor/ActorSystem.h>
#include <actor/ActorRequest.h>

namespace actor {

template <class PayloadInT, class PayloadRetT>
PayloadPtr<PayloadRetT> Actor::sendSync(PayloadPtr<PayloadInT> &&payload) {
    RequestPtr<ActorRequest> req;
    auto eb = system_->getEventBase();
    auto f1 = via(eb).then([this, &req, &payload, eb]() {
        DCHECK(eb->isInEventBaseThread());
        req = system_->getTracker()->allocRequest<ActorRequest>();
        auto future = req->from(system_->myId())
                          .toActor(myId())
                          .withPayload<PayloadInT>(payload)
                          .fire();
        return future;
    });
    f1.wait();
    return req->respPayloadBuffer<PayloadRetT>();
}

template <class MsgT>
void Actor::reply(Payload &&payload) {
    /* Current message for which we are replying must be normal direction */
    DCHECK(direction() == MSGDIRECTION_NORMAL);
    auto replyMsg = makeActorMsgCommon(MSGDIRECTION_RESPONSE,
                                       ActorMsgTypeEnum<MsgT>::typeId,
                                       myId_,
                                       from(),
                                       requestId(),
                                       std::move(payload)); 
    ROUTE(std::move(replyMsg));
}
}  // namespace actor

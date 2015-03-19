#include <Actor.h>

namespace actor {

void Actor::setId(const ActorId &id)
{
    id_ = id;
}

ActorId Actor::getId() const
{
    return id_;
}

NotificationQueueActor::NotificationQueueActor(folly::EventBase *eventBase) {
    this->startConsuming(eventBase, &queue_);
}

void NotificationQueueActor::send(ActorMsg &&msg) {
    queue_.putMessage(std::move(msg));
}

void NotificationQueueActor::messageAvailable(ActorMsg &&msg) {
    handle(std::move(msg));
}
}  // namespace actor

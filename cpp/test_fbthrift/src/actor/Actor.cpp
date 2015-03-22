#include <actor/Actor.h>
#include <sstream>

namespace actor {

Actor::Actor(ActorSystem *system) {
    system_ = system;
    currentBehavior_ = nullptr;
}

Actor::~Actor() {
}

void Actor::init() {
    initBehaviors_();
}

void Actor::initBehaviors_() {
    initBehavior_ = {
        on(Other) >> [this](ActorMsg &&m) {
            dropMessage(std::move(m));
        }
    };
    functionalBehavior_ = {
        on(Other) >> [this](ActorMsg &&m) {
            dropMessage(std::move(m));
        }
    };
    stoppedBehavior_ = {
        on(Other) >> [this](ActorMsg &&m) {
            dropMessage(std::move(m));
        }
    };
    inErrBehavior_ = {
        on(Other) >> [this](ActorMsg &&m) {
            dropMessage(std::move(m));
        }
    };
}


void Actor::setId(const ActorId &id)
{
    id_ = id;

    std::stringstream ss;
    ss << "[" << id.systemId << ":" << id.localId << "]";
    strId_ = ss.str();
}

ActorId Actor::getId() const
{
    return id_;
}

void Actor::changeBhavior(Behavior *behavior) {
    currentBehavior_ = behavior;
}

void Actor::handle(ActorMsg &&msg) {
    currentBehavior_->handle(std::move(msg));
}

NotificationQueueActor::NotificationQueueActor(ActorSystem *system,
                                               folly::EventBase *eventBase)
: Actor(system)
{
    setEventBase(eventBase);
}

void NotificationQueueActor::init() {
    Actor::init();
}

void NotificationQueueActor::setEventBase(folly::EventBase *eventBase) {
    this->startConsuming(eventBase, &queue_);
}

void NotificationQueueActor::send(ActorMsg &&msg) {
    queue_.putMessage(std::move(msg));
}

void NotificationQueueActor::messageAvailable(ActorMsg &&msg) {
    handle(std::move(msg));
}

void NotificationQueueActor::dropMessage(ActorMsg &&msg) {
    assert(!"TODO: Implemented");
}

void NotificationQueueActor::deferMessage(ActorMsg &&msg) {
    assert(!"TODO: Implemented");
}

}  // namespace actor

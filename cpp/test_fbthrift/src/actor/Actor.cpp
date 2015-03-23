#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <actor/Actor.h>
#include <actor/ActorSystem.h>
#include <sstream>

namespace actor {

Actor::Actor(ActorSystem *system)
{
    system_ = system;
    myId_ = ActorSystem::invalidActorId();
    currentBehavior_ = nullptr;
}

Actor::~Actor() {
}

void Actor::init() {
    ALog(INFO);
    initBehaviors_();
    send(makeActorMsg<Init>());
}

void Actor::initBehaviors_() {
    initBehavior_ = {
        on(Other) >> [this]() {
            dropMsg();
        }
    };
    functionalBehavior_ = {
        on(Other) >> [this]() {
            dropMsg();
        }
    };
    stoppedBehavior_ = {
        on(Other) >> [this]() {
            dropMsg();
        }
    };
    inErrBehavior_ = {
        on(Other) >> [this]() {
            dropMsg();
        }
    };
}

void Actor::setId(const ActorId &id) {
    myId_ = id;

    /* Set the string used for logging as well */
    std::stringstream ss;
    ss << "[" << id.systemId << ":" << id.localId << "]";
    strId_ = ss.str();
}

void Actor::changeBhavior(Behavior *behavior) {
    currentBehavior_ = behavior;
}

void Actor::handle(ActorMsg &&msg) {
    /* Cache current message.  This cached message is used in all the behavior
     * handlers
     */
    curMsg_ = &msg;

    CHECK(to() == myId());
    currentBehavior_->handle(actorMsgTypeId(msg));

    curMsg_ = nullptr;
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

void NotificationQueueActor::dropMsg() {
    CHECK(!"TODO: Implemented");
}

void NotificationQueueActor::deferMsg() {
    CHECK(!"TODO: Implemented");
}

}  // namespace actor

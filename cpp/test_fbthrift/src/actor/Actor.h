#pragma once

#include <string>
#include <folly/io/async/NotificationQueue.h>

namespace actor {

enum ActorMsgType {
    PUT,
    GET
};

struct AsyncHdr {
    ActorMsgType type;
    uint64_t from;
};

using PayloadPtr = std::shared_ptr<void>;

using ActorMsg = std::pair<AsyncHdr, PayloadPtr>;

using ActorId = int64_t;

struct Actor {
    void setId(const ActorId &id);
    ActorId getId() const;

    virtual void send(ActorMsg &&msg) = 0;
    virtual bool handle(ActorMsg &&msg) = 0;

protected:
    ActorId     id_;
};
using ActorPtr = std::shared_ptr<Actor>;

using ActorQueue = folly::NotificationQueue<ActorMsg>;

struct NotificationQueueActor : Actor, ActorQueue::Consumer {
    explicit NotificationQueueActor(folly::EventBase *eventBase);

    virtual void send(ActorMsg &&msg) override;
    virtual void messageAvailable(ActorMsg &&msg) override;

 protected:
    ActorQueue queue_;
};
}  // namespace actor

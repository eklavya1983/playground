#pragma once
#include <actor/Actor.hpp>
#include <actor/ActorRequest.h>
#include <actor/ActorSystem.h>

namespace volume {

using namespace actor;

struct Put: LocalPayload<> {};
struct Get: LocalPayload<> {};

struct VolumeReplicaHandle {
    void handle() {
    }
};

struct VolumeGroupHandle : NotificationQueueActor {
    using NotificationQueueActor::NotificationQueueActor;

    static const char* className() {return "VolumeGroupHandle";} 

 protected:
    virtual void initBehaviors_() override {
        NotificationQueueActor::initBehaviors_();
        initBehavior_ += {
            on(Init) >> [this]() {
                changeBehavior(&functionalBehavior_);
            }
        };
        functionalBehavior_ += {
            on(Put) >> [this]() {
            },
            onresp(Put) >> [this]() {
            }
        };
    }

    std::vector<VolumeReplicaHandle> volHandles_;
};


struct VolumeReplica {
};

}  // namespace volume

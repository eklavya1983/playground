#pragma once
#include <actor/Actor.h>
#include <actor/gen-cpp2/ServiceApi.h>

namespace actor {
using namespace cpp2;

struct RemoteActor : NotificationQueueActor {
    /* Lifecycle */
    RemoteActor(ActorSystem *system,
                folly::EventBase *eventBase);
    ~RemoteActor();

    static const char* className() {return "RemoteActor";} 

 protected:
    void initBehaviors_() override;
    
    ActorInfo info_;
    std::unique_ptr<ServiceApiAsyncClient> client_; 
};

}  // namespace actor

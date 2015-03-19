#pragma once

#include <Actor.h>
#include <ActorServer.h>
#include <folly/AtomicHashMap.h>

namespace actor {

struct ActorSystem {
    ActorSystem(int myPort,
                const std::string &configIp,
                int configPort);
    virtual ~ActorSystem();

    void init();

    /**
    * @brief Register actor with the actor system
    *
    * @param id
    * @param actorRef
    */
    void registerActor(const ActorId &id, const ActorPtr &actorRef);

    /**
    * @brief Return referece to the actor based on actor id
    *
    * @param id
    *
    * @return 
    */
    ActorPtr lookUpActor(const ActorId &id);

 protected:
    void register_();

    // ActorSystemId                               id_;
    using ActorTbl          = folly::AtomicHashMap<ActorId, ActorPtr>;
    using ActorTblPtr       = std::unique_ptr<ActorTbl>;
    ActorTblPtr                                 actorTbl_;
    ActorServerPtr                              server_;
};

}  // namespace actor 

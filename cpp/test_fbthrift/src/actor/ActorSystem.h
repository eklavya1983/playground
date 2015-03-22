#pragma once

#include <folly/AtomicHashMap.h>

#include <actor/Actor.h>
#include <actor/ActorUtil.h>
#include <actor/ActorServer.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

struct ActorSystem : NotificationQueueActor {
    /* Life cycle */
    ActorSystem(int myPort,
                const std::string &configIp,
                int configPort);
    virtual ~ActorSystem();
    virtual void init() override;

    /**
    * @brief Spawn root actor
    *
    * @tparam ActorT
    * @tparam ArgsT
    * @param args
    *
    * @return 
    */
    template<class ActorT, class ... ArgsT>
    ActorPtr spawnRootActor(ArgsT&&... args);

    /**
    * @brief Spawn the actor
    *
    * @tparam T
    * @tparam ArgsT
    * @param args
    *
    * @return 
    */
    template<class ActorT, class ... ArgsT>
    ActorPtr spawnActor(ArgsT&&... args);

    /**
    * @brief Return referece to the actor based on actor id
    *
    * @param id
    *
    * @return 
    */
    ActorPtr lookUpActor(const ActorId &id);

    /**
    * @brief Route message of type ActorMsg.  It's templated to use perferct forwarding
    *
    * @tparam MsgT must be ActorMsg
    * @param msg
    *
    * @return 
    */
    template <class MsgT>
    bool routeToActor(MsgT&& msg);

    void loop();

    void handleInitMsg(ActorMsg &&msg);
    void handleUpdateActorTableMsg(ActorMsg &&msg);

 protected:
    virtual void initBehaviors_() override;
    virtual void initSerializers_();

    void register_();
    
    ActorSystemId                       systemId_;
    std::atomic<LocalActorId>           nextLocalActorId_;
    using ActorTbl =                    folly::AtomicHashMap<int64_t, ActorPtr>;
    using ActorTblPtr =                 std::unique_ptr<ActorTbl>;
    ActorTblPtr                         actorTbl_;
    ActorServerPtr                      server_;


    // TODO: Event base management
    folly::EventBase                    eventBase_;
};
using ActorSystemPtr = std::shared_ptr<ActorSystem>;

}  // namespace actor 

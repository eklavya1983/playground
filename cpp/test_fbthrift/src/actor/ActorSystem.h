#pragma once

#include <folly/AtomicHashMap.h>

#include <actor/Actor.h>
#include <actor/ActorUtil.h>
#include <actor/ActorServer.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

struct ActorSystem : NotificationQueueActor {
    /* Life cycle */
    ActorSystem(const std::string &systemType,
                int myPort,
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

    inline static const ActorId& configActorId() {
        const static ActorId configId(apache::thrift::FRAGILE, 1, 1);
        return configId;
    }
    inline static const ActorId& invalidActorId() {
        const static ActorId invalidId(apache::thrift::FRAGILE, 0, 0);
        return invalidId;
    }
    inline static bool isActorSystemId(const ActorId &id) {
        return id.localId == LOCALID_START;
    }
    static const LocalActorId LOCALID_START = 0;


 protected:
    virtual void initBehaviors_() override;
    virtual ActorPtr createRemoteActor_(const ActorInfo &info);
    virtual Error updateActorRegistry_(const ActorInfo &info, bool createActor);
    void initConfigRemoteActor_();
    void sendRegisterMsg_();
    
    ActorInfo                           systemInfo_;
    std::string                         configIp_;
    int                                 configPort_;
    std::atomic<LocalActorId>           nextLocalActorId_;
    using ActorTbl =                    folly::AtomicHashMap<int64_t, ActorPtr>;
    using ActorTblPtr =                 std::unique_ptr<ActorTbl>;
    using ActorRegistry =               std::unordered_map<int64_t, ActorInfo>;
    ActorTblPtr                         actorTbl_;
    ActorRegistry                       actorRegistry_; 
    ActorServerPtr                      server_;
    // TODO: Event base management
    folly::EventBase                    eventBase_;
};
using ActorSystemPtr = std::shared_ptr<ActorSystem>;

}  // namespace actor 

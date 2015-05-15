#pragma once

#include <folly/AtomicHashMap.h>
#include <folly/wangle/concurrent/IOThreadPoolExecutor.h>

#include <actor/Actor.h>
#include <actor/ActorUtil.h>
#include <actor/ActorServer.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

struct ActorSystem : NotificationQueueActor {
    /* Life cycle */
    ActorSystem();
    ActorSystem(const ActorId &id,
                const std::string &systemType,
                std::unique_ptr<ServiceApiSvIf> handler,
                int myPort,
                const std::string &configIp,
                int configPort);
    virtual ~ActorSystem();

    virtual void init() override;

    /**
    * @brief Registers with configuration servicer.  Registration is performed in synchronous
    * manner
    */
    virtual void registerWithConfigService();

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

    folly::EventBase* getNextEventBase();
    std::shared_ptr<folly::wangle::IOThreadPoolExecutor> getIOThreadPool();

    /* Exposed so that message to behavior logging is more useful */
    static const char* className() {return "ActorSystem";} 

    inline bool isRemoteId(const ActorId& id) {
        return id.systemId != myId_.systemId;
    }
    inline ActorId toRemoteId(const ActorId& id) {
        return ActorId(apache::thrift::FRAGILE, id.systemId, LOCALID_START);
    }
    inline static const ActorId& configActorId() {
        const static ActorId configId(apache::thrift::FRAGILE, 1, 0);
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
    ActorTbl                            actorTbl_;
    ActorRegistry                       actorRegistry_; 
    ActorServerPtr                      server_;
    std::shared_ptr<folly::wangle::IOThreadPoolExecutor> ioThreadPool_;
};
using ActorSystemPtr = std::shared_ptr<ActorSystem>;

}  // namespace actor 

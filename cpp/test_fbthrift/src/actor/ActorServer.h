#pragma once

#include <string>
#include <thrift/lib/cpp/util/ScopedServerThread.h>
#include <actor/gen-cpp2/ServiceApi.h>
#include <actor/ActorMsg.h>

#define MAP_SERAILIZER(enumType, msgType) \
    ServiceHandler::addActorMsgSerializer<msgType>(static_cast<ActorMsgType>(enumType))
#define ADD_SERAILIZER(msgType) \
    ServiceHandler::addActorMsgSerializer<msgType>(\
                        static_cast<ActorMsgType>(ActorMsgTypes::msgType##Msg))

namespace actor {

using namespace cpp2;

/* Forward declarations */
struct ActorSystem;

struct ActorServer {
    virtual void start() = 0;
    virtual void stop() = 0;
};
using ActorServerPtr = std::shared_ptr<ActorServer>;

struct ReplicaActorServer : ActorServer {
    explicit ReplicaActorServer(ActorSystem *system, int port);
    virtual void start() override;
    virtual void stop() override;
 protected:
    ActorSystem *system_;
    int port_;
    std::unique_ptr<apache::thrift::util::ScopedServerThread> serverThread_;
};

/**
* @brief Thrift cpp2 handler for handling async requests
*/
struct ServiceHandler : ::actor::cpp2::ServiceApiSvIf {
    /* Typedefs */
    using SerializerF = std::function<void (const ActorMsg&, std::unique_ptr<folly::IOBuf>&)>;
    using DeserializerF = std::function<void (const std::unique_ptr<folly::IOBuf>&, ActorMsg&)>;
    using SerializerTbl = std::unordered_map<ActorMsgType, std::pair<SerializerF, DeserializerF>>;

    explicit ServiceHandler(ActorSystem *system);
    virtual void actorMessage(std::unique_ptr<ActorMsgHeader> header,
                              std::unique_ptr<folly::IOBuf> payload) override;
    virtual void replicaRequest(std::unique_ptr<ReplicaRequestHeader> header,
                                std::unique_ptr<std::string> payload) override;
    template <class MsgT>
    static void addActorMsgSerializer(ActorMsgType type) {
        actorMsgSerializerTbl_[type] = {&toIOBuf<MsgT>, &toActorMsg<MsgT>};
    }
    inline static SerializerTbl& getSerializerTbl() {return actorMsgSerializerTbl_;}

 protected:
    static SerializerTbl actorMsgSerializerTbl_;

    ActorSystem *system_;
    friend struct ActorServer;
};

}  // namespace actor

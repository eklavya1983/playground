#pragma once

#include <string>
#include <thrift/lib/cpp/util/ScopedServerThread.h>
#include <actor/gen-cpp2/ServiceApi.h>
#include <actor/ActorMsg.h>

namespace apache { namespace thrift {
class ThriftServer;
}}

namespace actor {

using namespace cpp2;

/* Forward declarations */
struct ActorSystem;

struct ActorServer {
    virtual ~ActorServer() {}
    virtual void start(bool block) = 0;
    virtual void stop() = 0;
};
using ActorServerPtr = std::shared_ptr<ActorServer>;

/**
* @brief Thrift cpp2 handler for handling async requests
*/
struct ServiceHandler : virtual ::actor::cpp2::ServiceApiSvIf {

    explicit ServiceHandler(ActorSystem *system);
    virtual void actorMessage(std::unique_ptr<ActorMsgHeader> header,
                              std::unique_ptr<folly::IOBuf> payload) override;
 protected:

    ActorSystem *system_;
    friend struct ActorServer;
};


struct ReplicaActorServer : ActorServer {
    ReplicaActorServer(ActorSystem *system, int nIoThreads, int port);
    ReplicaActorServer(ActorSystem *system,
                       std::unique_ptr<ServiceHandler> handler,
                       int nIoThreads, int port);
    virtual void start(bool block) override;
    virtual void stop() override;
 protected:
    ActorSystem *system_;
    int port_;
    std::shared_ptr<apache::thrift::ThriftServer> server_;
    std::unique_ptr<apache::thrift::util::ScopedServerThread> serverThread_;
};

}  // namespace actor

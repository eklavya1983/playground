#pragma once

#include <string>
#include <thrift/lib/cpp/util/ScopedServerThread.h>
#include <actor/gen-cpp2/ServiceApi.h>

namespace actor {

using namespace cpp2;

/* Forward declarations */
struct ActorSystem;

/**
* @brief Thrift cpp2 handler for handling async requests
*/
struct ServiceHandler : ::actor::cpp2::ServiceApiSvIf {
    explicit ServiceHandler(ActorSystem *system);
    virtual void actorMessage(std::unique_ptr<ActorMsgHeader> header,
                              std::unique_ptr<folly::IOBuf> payload) override;
    virtual void replicaRequest(std::unique_ptr<ReplicaRequestHeader> header,
                                std::unique_ptr<std::string> payload) override;
 protected:
    ActorSystem *system_;
};

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
}  // namespace actor

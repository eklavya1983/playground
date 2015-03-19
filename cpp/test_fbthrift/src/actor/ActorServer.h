#pragma once

#include <string>
#include <thrift/lib/cpp/util/ScopedServerThread.h>
#include <ServiceApi.h>

namespace actor {
struct ServiceHandler : ::actor::cpp2::ServiceApiSvIf {
    virtual void replicaRequest(std::unique_ptr< ::actor::cpp2::ReplicaRequestHeader> header,
                                std::unique_ptr<std::string> payload) override;
};

struct ActorServer {
    virtual void start() = 0;
    virtual void stop() = 0;
};
using ActorServerPtr = std::shared_ptr<ActorServer>;

struct ReplicaActorServer : ActorServer {
    explicit ReplicaActorServer(int port);
    virtual void start() override;
    virtual void stop() override;
 protected:
    int port_;
    std::unique_ptr<apache::thrift::util::ScopedServerThread> serverThread_;
};
}  // namespace actor

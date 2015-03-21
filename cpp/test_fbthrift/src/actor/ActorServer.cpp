#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <actor/ActorSystem.hpp>
#include <actor/ActorServer.h>

namespace actor {

ServiceHandler::ServiceHandler(ActorSystem *system) {
   system_ = system; 
}

void ServiceHandler::actorMessage(std::unique_ptr<ActorMsgHeader> header,
                                  std::unique_ptr<folly::IOBuf> payload) {
    // TODO: Check if move is succesful or not
    bool ret = system_->routeToActor(
        ActorMsg(*header, std::make_shared<Payload>(std::move(payload))));
    if (!ret) {
        // TODO: log an error
    }
}

void ServiceHandler::replicaRequest(std::unique_ptr<ReplicaRequestHeader> header,
                                    std::unique_ptr<std::string> payload) {
}

ReplicaActorServer::ReplicaActorServer(ActorSystem *system, int port)
{
    system_ = system;
    port_ = port;
}

void ReplicaActorServer::start() {
    std::shared_ptr<apache::thrift::ThriftServer> server(new apache::thrift::ThriftServer);
    if (true) {
        std::shared_ptr<apache::thrift::concurrency::ThreadFactory> threadFactory(
            new apache::thrift::concurrency::PosixThreadFactory);
        std::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager(
            apache::thrift::concurrency::ThreadManager::newSimpleThreadManager(
                1, 1000, false, 0));
        threadManager->threadFactory(threadFactory);
        threadManager->start();
        server->setThreadManager(threadManager);
    }
    server->setPort(port_);
    server->setInterface(std::unique_ptr<ServiceHandler>(new ServiceHandler(system_)));

    serverThread_.reset(new apache::thrift::util::ScopedServerThread(server));
}

void ReplicaActorServer::stop() {
    serverThread_.reset();
}

}  // namespace actor

#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <ActorServer.h>

namespace actor {

void ServiceHandler::replicaRequest(
    std::unique_ptr< ::actor::cpp2::ReplicaRequestHeader> header,
    std::unique_ptr<std::string> payload) {
}

ReplicaActorServer::ReplicaActorServer(int port)
{
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
    server->setInterface(std::unique_ptr<ServiceHandler>(new ServiceHandler));

    serverThread_.reset(new apache::thrift::util::ScopedServerThread(server));
}

void ReplicaActorServer::stop() {
    serverThread_.reset();
}

}  // namespace actor

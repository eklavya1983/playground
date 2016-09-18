#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <util/Log.h>
#include <actor/ActorSystem.hpp>
#include <actor/ActorServer.h>

namespace actor {
ReplicaActorServer::ReplicaActorServer(ActorSystem *system,
                       int nIoThreads, int port)
    : ReplicaActorServer(system, nullptr, nIoThreads, port)
{
}

ReplicaActorServer::ReplicaActorServer(ActorSystem *system,
                                       std::unique_ptr<ServiceApiSvIf> handler,
                                       int nIoThreads, int port)
{
    system_ = system;
    port_ = port;
    if (handler == nullptr) {
        handler.reset(new ServiceHandler(system_));
    }
    server_.reset(new apache::thrift::ThriftServer("disabled", false));
    server_->setPort(port_);
    server_->setInterface(std::move(handler));
    // server_->setIOThreadPool(system_->getIOThreadPool());
    server_->setNWorkerThreads(nIoThreads);
}

void ReplicaActorServer::start(bool block) {
    if (true) {
        std::shared_ptr<apache::thrift::concurrency::ThreadFactory> threadFactory(
            new apache::thrift::concurrency::PosixThreadFactory);
        std::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager(
            apache::thrift::concurrency::ThreadManager::newSimpleThreadManager(
                1, 1000, false, 0));
        threadManager->threadFactory(threadFactory);
        threadManager->start();
        server_->setThreadManager(threadManager);
    }

    if (block) {
        server_->serve();
    } else {
        serverThread_.reset(new apache::thrift::util::ScopedServerThread(server_));
    }
}

void ReplicaActorServer::stop() {
    if (serverThread_) {
        serverThread_.reset();
    }
}


ServiceHandler::ServiceHandler(ActorSystem *system) {
   system_ = system; 
}

void ServiceHandler::actorMessage(std::unique_ptr<ActorMsgHeader> header,
                                  std::unique_ptr<folly::IOBuf> payload) {
    /* deserialize */
    auto msg = ActorMsg(*header, nullptr);
    auto &msgDeserializerF = gMsgTypeInfoTbl->at(msg.typeId()).deserializer;
    msgDeserializerF(payload, msg);

    /* route */
    ROUTE(std::move(msg));
}

ActorSystem* ServiceHandler::getActorSystem() {
    return system_;
}
}  // namespace actor

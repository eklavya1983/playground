#include <util/Log.h>
#include <infra/ZooKafkaClient.h>
#include <infra/Service.h>
#include <infra/gen/gen-cpp2/ServiceApi.h>
#include <infra/ServiceServer.h>

namespace infra {

struct ServiceApiHandler : cpp2::ServiceApiSvIf {
    void getModuleState(std::string& _return,
                        std::unique_ptr<std::map<std::string, std::string>> arguments) override {
        _return = "ok";
        LOG(INFO) << "returning hello";
    }
};

Service* Service::newDefaultService(const std::string &logContext,
                                    const ServiceInfo &info,
                                    const std::string &zkServers)
{
    auto zkClient = std::make_shared<ZooKafkaClient>(logContext, zkServers);
    auto service = new Service(logContext, info, zkClient);
    return service;
}

Service::Service(const std::string &logContext,
                 const ServiceInfo &info,
                 const std::shared_ptr<CoordinationClient> &coordinationClient)
    : logContext_(logContext),
    serviceInfo_(info)
{
    coordinationClient_ = coordinationClient;
    server_ = std::make_shared<ServiceServer>(logContext,
                                              serviceInfo_.ip,
                                              serviceInfo_.port,
                                              std::make_shared<ServiceApiHandler>());
}


Service::~Service()
{
}


void Service::init()
{
    // TODO(Rao): initing
    // initCoordinationClient_();
    // initPubSub_();
    initServer_();

    /* init other modules */
}

void Service::run()
{
    server_->start();
}

void Service::shutdown()
{
    server_->stop();
}

void Service::initCoordinationClient_()
{
    /* Connect with zookeeper */
    coordinationClient_->init();
    CLog(INFO) << "connected established with zookeeper";

    /* Pull current properties */
    /* Pull service cache */
}

void Service::initServer_()
{
    // TODO(Rao): Any sort of intiting
}

}  // namespace infra

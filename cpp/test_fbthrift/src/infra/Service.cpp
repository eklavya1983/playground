#include <util/Log.h>
#include <infra/ZooKafkaClient.h>
#include <infra/Service.h>
#include <infra/gen/gen-cpp2/ServiceApi.h>
#include <infra/gen/configtree_constants.h>
#include <infra/ServiceServer.h>
#include <infra/Serializer.tcc>
#include <zookeeper.h>   /* Included just ZNONODE error */

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
    auto zkClient = std::make_shared<ZooKafkaClient>(logContext, zkServers, info.id);
    auto service = new Service(logContext, info, true, zkClient);
    return service;
}

Service::Service(const std::string &logContext,
                 const ServiceInfo &info,
                 bool enableServer,
                 const std::shared_ptr<CoordinationClient> &coordinationClient)
    : logContext_(logContext),
    serviceInfo_(info)
{
    coordinationClient_ = coordinationClient;
    if (enableServer) {
        server_ = std::make_shared<ServiceServer>(logContext,
                                                  serviceInfo_.ip,
                                                  serviceInfo_.port,
                                                  std::make_shared<ServiceApiHandler>());
    }
}


Service::~Service()
{
}


void Service::init()
{
    if (coordinationClient_) {
        initCoordinationClient_();

        /* ensure service is part of the data sphere */
        ensureDatasphereMembership_();
        /* fetch properties */
        /* init connection cache */
    } else {
        CLog(INFO) << "coordination client is disabled.  Not initializing coordination client";
    }


    if (server_) {
        initServer_();
    } else {
        CLog(INFO) << "server is disabled.  Not initializing server";
    }

    /* publish this service is up */
    if (coordinationClient_) {
#if 0
        std::string payload;
        serializeToThriftJson<>(serviceInfo_, payload, getLogContext());
        auto f = coordinationClient_->set();
        f.wait();

        serializeToThriftJson<ServiceInfo>(serviceInfo_, payload, getLogContext());
        coordinationClient_->publishMessage()
#endif
    }
}

void Service::run()
{
    server_->start();
}

void Service::shutdown()
{
    server_->stop();
}

std::string Service::getDatasphereId() const
{
    return serviceInfo_.dataSphereId;
}

std::string Service::getNodeId() const
{
    return serviceInfo_.nodeId;
}

std::string Service::getServiceId() const
{
    return serviceInfo_.id;
}

CoordinationClient* Service::getCoordinationClient() const
{
    return coordinationClient_.get();
}

ConnectionCache* Service::getConnectionCache() const
{
    return nullptr;
}

void Service::initCoordinationClient_()
{
    /* Connect with zookeeper */
    coordinationClient_->init();
    CLog(INFO) << "connected established with zookeeper";
}

void Service::initServer_()
{
    // TODO(Rao): Any sort of intiting
}

void Service::ensureDatasphereMembership_()
{
    auto serviceEntryKey = folly::sformat(g_configtree_constants.SERVICE_ROOT_PATH_FORMAT,
                                          getDatasphereId(),
                                          getServiceId());
    try {
        coordinationClient_->get(serviceEntryKey).get();
    } catch (const ZookeeperException &e) {
        // TODO(Rao): We are leaking zookeeper specific error codes.  We
        // shouldn't
        if (e.getError() == ZNONODE) {
            CLog(FATAL) << "Service entry doesn't exist in configuration client";
        }
        throw;
    }
}

}  // namespace infra

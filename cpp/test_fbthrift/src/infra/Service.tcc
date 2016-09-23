#pragma once

#include <util/Log.h>
#include <infra/ZookeeperClient.h>
#include <infra/Service.h>
#include <infra/gen/gen-cpp2/ServiceApi.h>
#include <infra/ServiceServer.h>

#define SERVICE_TEMPL class ConfigurationClientT, class PubSubClientT
#define SERVICE_TEMPL_ARGS ConfigurationClientT, PubSubClientT

namespace infra {

struct ServiceApiHandler : cpp2::ServiceApiSvIf {
    void getModuleState(std::string& _return,
                        std::unique_ptr<std::map<std::string, std::string>> arguments) override {
        _return = "ok";
        LOG(INFO) << "returning hello";
    }
};


template<SERVICE_TEMPL>
Service<SERVICE_TEMPL_ARGS>::Service(const std::string &logContext,
                                     const std::string &serviceId,
                                     const std::string &ip,
                                     const int32_t &port,
                                     const std::string &zkServers)
    : logContext_(logContext),
    serviceId_(serviceId),
    ip_(ip),
    port_(port)
{
    configClient_ = std::make_shared<ZookeeperClient>(logContext_,
                                                      zkServers);
    server_ = std::make_shared<ServiceServer>(logContext,
                                              ip,
                                              port,
                                              std::make_shared<ServiceApiHandler>());
}

template<SERVICE_TEMPL>
Service<SERVICE_TEMPL_ARGS>::~Service()
{
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::init()
{
    // TODO(Rao): initing
    // initConfigurationClient_();
    // initPubSub_();
    initServer_();

    /* init other modules */
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::run()
{
    server_->start();
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::shutdown()
{
    server_->stop();
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::initConfigurationClient_()
{
    /* Connect with zookeeper */
    configClient_->init();
    configClient_->blockUntilConnectedOrTimedOut(5);
    CLog(INFO) << "connected established with zookeeper";
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::initPubSub_()
{
}

template<SERVICE_TEMPL>
void Service<SERVICE_TEMPL_ARGS>::initServer_()
{
    // TODO(Rao): Any sort of intiting
}

}  // namespace infra

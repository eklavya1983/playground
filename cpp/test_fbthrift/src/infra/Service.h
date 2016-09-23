#pragma once

#include <memory>

namespace apache { namespace thrift { namespace server {
class TNonblockingServer;
}}}

namespace infra {

struct ZookeeperClient;
struct KafkaClient;
struct ServiceServer;

/**
 * @brief Base Service class
 */
template<class ConfigurationClientT = ZookeeperClient,
        class PubSubClientT = KafkaClient>
struct Service {
    Service(const std::string &logContext,
            const std::string &serviceId,
            const std::string &ip,
            const int32_t &port,
            const std::string &zkServers);
    virtual ~Service();
    Service(const Service&) = delete;
    void operator=(Service const &) = delete;

    virtual void init();
    virtual void run();
    virtual void shutdown();

    inline std::string getLogContext() const {
        return logContext_;
    }

 protected:
    virtual void initConfigurationClient_();
    virtual void initPubSub_();
    virtual void initServer_();

    std::string                                 logContext_;
    std::string                                 serviceId_;
    std::string                                 ip_;
    int32_t                                     port_;
    /* Configuration db client like zookeeper */
    std::shared_ptr<ConfigurationClientT>       configClient_;
    /* Kafka client */

    /* Server */
    std::shared_ptr<ServiceServer>              server_;

    friend struct ServiceApiHandler;
};

}

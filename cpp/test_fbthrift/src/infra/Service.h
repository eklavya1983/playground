#pragma once

#include <memory>
#include <infra/ModuleProvider.h>
#include <infra/gen/commontypes_types.h>

namespace apache { namespace thrift { namespace server {
class TNonblockingServer;
}}}

namespace infra {

struct ServiceServer;
struct CoordinationClient;
struct ConnectionCache;

/**
 * @brief Base Service class
 */
struct Service : ModuleProvider {
    Service(const std::string &logContext,
            const ServiceInfo &info,
            bool enableServer,
            const std::shared_ptr<CoordinationClient> &coordinationClient);
    virtual ~Service();
    Service(const Service&) = delete;
    void operator=(Service const &) = delete;

    virtual void init();
    virtual void run();
    virtual void shutdown();

    const std::string& getServiceEntryKey() const;
    std::string getDatasphereId() const override;
    std::string getNodeId() const override;
    std::string getServiceId() const override;
    CoordinationClient* getCoordinationClient() const override;
    ConnectionCache*    getConnectionCache() const override;

    inline std::string getLogContext() const {
        return logContext_;
    }

    static Service* newDefaultService(const std::string &logContext,
                                      const ServiceInfo &info,
                                      const std::string &zkServers);

 protected:
    virtual void initCoordinationClient_();
    virtual void initServer_();
    virtual void ensureDatasphereMembership_();
    virtual void publishServiceInfomation_();

    std::string                                 logContext_;
    ServiceInfo                                 serviceInfo_;
    std::string                                 serviceEntryKey_;
    /* client for coordination services */
    std::shared_ptr<CoordinationClient>         coordinationClient_;
    /* Connection cache */
    std::shared_ptr<ConnectionCache>            connectionCache_;
    /* Server */
    std::shared_ptr<ServiceServer>              server_;

    friend struct ServiceApiHandler;
};

}
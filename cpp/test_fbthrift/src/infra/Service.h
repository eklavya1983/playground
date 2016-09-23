#pragma once

#include <memory>
#include <infra/gen/commontypes_types.h>

namespace apache { namespace thrift { namespace server {
class TNonblockingServer;
}}}

namespace infra {

struct ServiceServer;
struct CoordinationClient;

/**
 * @brief Base Service class
 */
struct Service {
    Service(const std::string &logContext,
            const ServiceInfo &info,
            const std::shared_ptr<CoordinationClient> &coordinationClient);
    virtual ~Service();
    Service(const Service&) = delete;
    void operator=(Service const &) = delete;

    virtual void init();
    virtual void run();
    virtual void shutdown();

    inline std::string getLogContext() const {
        return logContext_;
    }

    static Service* newDefaultService(const std::string &logContext,
                                      const ServiceInfo &info,
                                      const std::string &zkServers);

 protected:
    virtual void initCoordinationClient_();
    virtual void initServer_();

    std::string                                 logContext_;
    ServiceInfo                                 serviceInfo_;
    /* client for coordination services */
    std::shared_ptr<CoordinationClient>        coordinationClient_;

    /* Server */
    std::shared_ptr<ServiceServer>              server_;

    friend struct ServiceApiHandler;
};

}

#pragma once
#include <string>
#include <gflags/gflags.h>
#include <memory>

DECLARE_string(toolsdir);

namespace infra {
struct ServiceInfo;
struct DataSphereInfo;
struct CoordinationClient;
struct Service;
}

namespace testlib {

struct ConfigService;

struct DatomBringupHelper {
    DatomBringupHelper();
    virtual ~DatomBringupHelper() = default;
    void cleanStartDatom();
    void shutdownDatom();
    void addDataSphere(const std::string &dataSphereId);
    void addService(const std::string &dataSphereId,
                    const std::string &nodeId,
                    const std::string &serviceId,
                    const std::string &ip,
                    const int port);

 protected:
    std::string                                     taskScript_;
    std::shared_ptr<ConfigService>                  configService_;
};

}  // namespace

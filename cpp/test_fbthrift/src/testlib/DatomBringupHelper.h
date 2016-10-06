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
    DatomBringupHelper(const DatomBringupHelper &) = delete;
    void operator=(DatomBringupHelper const &) = delete;
    void cleanStartDatom();
    void cleanStopDatom();
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

/**
 * @brief RAII helper to clean start and clean stop datom
 */
struct ScopedDatom {
    ScopedDatom(DatomBringupHelper& d);
    ~ScopedDatom();
    ScopedDatom(const ScopedDatom&) = delete;
    void operator=(ScopedDatom const &) = delete;

 private:
    DatomBringupHelper &datom_;
};

}  // namespace

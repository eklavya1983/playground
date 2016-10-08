#pragma once
#include <string>
#include <memory>
#include <testlib/KafkaRunner.h>


namespace infra {
struct ServiceInfo;
struct DataSphereInfo;
struct CoordinationClient;
struct Service;
}


namespace testlib {
template <class ConfigServiceT>
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
    inline ConfigServiceT* getConfigService() { return configService_.get(); }

 protected:
    KafkaRunner                                     KafkaRunner_;
    std::shared_ptr<ConfigServiceT>                 configService_;
};

/**
 * @brief RAII helper to clean start and clean stop datom
 */
template <class ConfigServiceT>
struct ScopedDatom {
    ScopedDatom(DatomBringupHelper<ConfigServiceT>& d);
    ~ScopedDatom();
    ScopedDatom(const ScopedDatom&) = delete;
    void operator=(ScopedDatom const &) = delete;

 private:
    DatomBringupHelper<ConfigServiceT> &datom_;
};

}  // namespace testlib

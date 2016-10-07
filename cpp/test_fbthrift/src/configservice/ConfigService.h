#pragma once

#include <unordered_map>
#include <infra/InfraForwards.h>
#include <infra/Service.h>

namespace config {

struct DatasphereConfigMgr;
using DatasphereConfigMgrSPtr = std::shared_ptr<DatasphereConfigMgr>;

using namespace infra;

struct ConfigService : Service {
    using Service::Service;

    virtual ~ConfigService();

    void init() override;

    /* NOTE: Below methods don't do checks for existence.  It is intentional as
     * this code is just testing.  It is up to the user to ensure this isn't
     * exploited
     */
    void createDatom();
    void addDataSphere(const DataSphereInfo &info);
    void addService(const ServiceInfo &info);
    void startVolumeCluster();
    void startDataCluster();

 protected:
    void ensureDatasphereMembership_() override;

    bool                                                        datomConfigured_ {false};
    std::unordered_map<std::string, DatasphereConfigMgrSPtr>    datasphereTable_;
};

}  // namespace config

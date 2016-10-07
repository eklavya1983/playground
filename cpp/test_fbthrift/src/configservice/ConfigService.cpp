#include <configservice/ConfigService.h>
#include <folly/futures/Future.h>
#include <folly/Format.h>
#include <util/Log.h>
#include <infra/ZooKafkaClient.h>
#include <infra/Service.h>
#include <infra/Serializer.tcc>
#include <infra/gen/configtree_constants.h>
#include <infra/gen-ext/commontypes_ext.h>
#include <infra/ConnectionCache.h>
#include <infra/StatusException.h>
#include <infra/gen/status_types.h>

namespace config {

struct DatasphereConfigMgr {
    DatasphereConfigMgr(ConfigService *parent, const DataSphereInfo &info)
        : parent_(parent),
        datasphereInfo_(info)
    {
    }
    virtual ~DatasphereConfigMgr() = default;

    const std::string& getLogContext() const { return parent_->getLogContext(); }

    void addService(const ServiceInfo &info)
    {
        /* Serialize to TJson */
        std::string payload;
        serializeToThriftJson<ServiceInfo>(info, payload, getLogContext());

        /* Do a Put */
        auto f = parent_->getCoordinationClient()->createIncludingAncestors(
            folly::sformat(configtree_constants::SERVICE_ROOT_PATH_FORMAT(),
                           info.dataSphereId, info.id),
            payload);
        f.get();
        CLog(INFO) << "Added service " << info;
    }
 protected:
    ConfigService                                   *parent_;
    DataSphereInfo                                  datasphereInfo_;
    std::unordered_map<std::string, ServiceInfo>    serviceCache_;
};

ConfigService::~ConfigService()
{
    CLog(INFO) << "Exiting ConfigService";
}

void ConfigService::init()
{
    serviceEntryKey_ = configtree_constants::CONFIGSERVICE_ROOT();

    initCoordinationClient_();
    /* Init connection cache, etc */
    connectionCache_->init();

    /* Check if datom is configured */
    auto f = coordinationClient_->get(getServiceEntryKey());
    try {
        (void) f.get();
        datomConfigured_ = true;
    } catch (const StatusException &e) {
        if (e.getStatus() == Status::STATUS_INVALID_KEY) {
            datomConfigured_ = false;
            CLog(INFO) << "Datom is not yet configured.  Will wait";
            return;
        } else {
            CLog(ERROR) << "Failed to get datom information.  Init failed";
            throw;
        }
    }
    CLog(INFO) << "Datom is already configured";

    publishServiceInfomation_();

}

void ConfigService::createDatom()
{
    /* Do a create */
    auto f = coordinationClient_->create(configtree_constants::DATOM_ROOT(), "");
    f.get();
    CLog(INFO) << "Created datom root at:" << configtree_constants::DATOM_ROOT();
    f = coordinationClient_->create(getServiceEntryKey(), "");
    CLog(INFO) << "Created config service root at:" << getServiceEntryKey();

    publishServiceInfomation_();
}

void ConfigService::addDataSphere(const DataSphereInfo &info)
{
    /* Serialize to TJson */
    std::string payload;
    serializeToThriftJson<DataSphereInfo>(info, payload, getLogContext());

    /* Do a create */
    auto f = coordinationClient_->createIncludingAncestors(
        folly::sformat(configtree_constants::DATASPHERE_ROOT_PATH_FORMAT(), info.id),
        payload);
    f.get();

    /* Update in memory cache */
    datasphereTable_[info.id] = std::make_shared<DatasphereConfigMgr>(this, info);

    CLog(INFO) << "Added new datasphere id:" << info.id;
}

void ConfigService::addService(const ServiceInfo &info)
{
    /* lookup datasphere */
    auto datasphere = datasphereTable_.find(info.dataSphereId);
    if (datasphere == datasphereTable_.end()) {
        throw StatusException(Status::STATUS_INVALID_DATASPHERE);
    }
    /* add to datasphere */
    datasphere->second->addService(info);

}

void ConfigService::startVolumeCluster()
{
}

void ConfigService::startDataCluster()
{
}

void ConfigService::ensureDatasphereMembership_()
{
    /* No need to check for memebership for config service */
}

}  // namespace config

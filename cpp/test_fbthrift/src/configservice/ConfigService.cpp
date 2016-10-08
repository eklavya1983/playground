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
#include <infra/MessageException.h>
#include <infra/gen/status_types.h>
#include <infra/LockHelper.tcc>

#include <folly/io/async/EventBase.h>
#include <infra/gen/configtree_constants.h>

namespace config {

using namespace infra;
using folly::via;

template<class ConfigServiceT, class ResourceT>
struct PBResourceSphereConfigMgr {
    PBResourceSphereConfigMgr(ConfigServiceT *parent,
                              const std::string &datasphereId,
                              int32_t replicaFactor,
                              folly::EventBase *eb)
    {
        parent_ = parent;
        datasphereId_ = datasphereId;
        replicaFactor_ = replicaFactor;
        eb_ = eb;
        nextAddRingId_ = 0;
    }

    inline const std::string& getLogContext() const {
        return parent_->getLogContext();
    }

    virtual VoidFuture addService(const ServiceInfo &info) {
        return via(eb_).then([this, info]() {
            pendingServices_.push_back(info.id);
            if (pendingServices_.size() >= replicaFactor_) {
                // TODO(Rao): Support adding services when pending is >
                // replicaFactor_
                CHECK(pendingServices_.size() == replicaFactor_);
                RingInfo ring;
                ring.id = nextAddRingId_++;
                ring.memberIds = std::move(pendingServices_);
                pendingServices_.clear();
                auto f = addRing_(ring);
                f
                .onError([this, ring](const StatusException &e) {
                    // TODO(Rao): Handle this
                    CHECK(!"Failed to add.  Currenlty not handled");
                    CLog(INFO) << "failed to add service: " << e.what();
                 });
                return f;
            }
            return;
        });
    }

    virtual folly::Future<ResourceT> addResource(const ResourceT &info)
    {
        return via(eb_).then([this, info]() {
            if (rings_.size() == 0) {
                CLog(WARNING) << "Failed to add. Ring table size is zero";
                throw MessageException(Status::STATUS_INVALID, "Zero size service rings");
            }

            throwOnPreAddResourceCheckFailures_(info);

            auto resource = info;
            resource.id = nextResourceId_++;
            resource.ringId = resource.id % rings_.size();

            auto resourceRoot = folly::sformat(
                configtree_constants::PB_SPHERE_RESOURCE_ROOT_PATH_FORMAT(),
                datasphereId_, pbsphereId_, resource.id);
            auto payload = serializeToThriftJson<>(resource, getLogContext());

            auto f = parent_->getCoordinationClient()->\
            createIncludingAncestors(resourceRoot, payload)
            .via(eb_)
            .then([this, resource](const std::string& data) {
                  resources_[resource.id] = resource;
                  CLog(INFO) << "added resource " << resource;
                  return resource;
            });

            return f;
         });
    }

 protected:
    virtual VoidFuture addRing_(const RingInfo &ring)
    {
        DCHECK(eb_->isInEventBaseThread());
        auto ringRoot = folly::sformat(configtree_constants::PB_SPHERE_RING_ROOT_PATH_FORMAT(),
                                       datasphereId_, ring.id);
        auto payload = serializeToThriftJson<>(ring, getLogContext());
        auto f = parent_->getCoordinationClient()->createIncludingAncestors(ringRoot, payload);
        return f
            .via(eb_)
            .then([this, ring](const std::string& data) {
                rings_[ring.id] = ring;
                CLog(INFO) << "successfully added ring " << ring;
            });
    }

    virtual void throwOnPreAddResourceCheckFailures_(const ResourceT &info)
    {
    }

    ConfigServiceT                              *parent_;
    std::string                                 datasphereId_;
    std::string                                 pbsphereId_;
    int32_t                                     replicaFactor_;
    folly::EventBase                            *eb_;
    /* Pending services from which ring can be created */
    std::vector<std::string>                    pendingServices_;
    /* Table for rings */
    std::unordered_map<int32_t, RingInfo>       rings_;
    /* Id for the next to be added ring */
    int32_t                                     nextAddRingId_;
    /* Table for volumes */
    std::unordered_map<int64_t, ResourceT>      resources_;
    /* Id for the next to be added resource */
    int64_t                                     nextResourceId_;
};

using PBVolumeConfigMgr = PBResourceSphereConfigMgr<ConfigService, VolumeInfo>;

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

        /* Update configdb */
        auto f = parent_->getCoordinationClient()->createIncludingAncestors(
            folly::sformat(configtree_constants::SERVICE_ROOT_PATH_FORMAT(),
                           info.dataSphereId, info.id),
            payload);
        f.get();

        /* Update cache */
        {
            std::unique_lock<folly::SharedMutex> l(serviceCacheMutex_);
            serviceCache_[info.id] = info;
        }
        CLog(INFO) << "Added service " << info;
    }

 protected:
    ConfigService                                   *parent_;
    DataSphereInfo                                  datasphereInfo_;
    using ServiceInfoTable = std::unordered_map<std::string, ServiceInfo>;
    folly::SharedMutex                              serviceCacheMutex_;
    ServiceInfoTable                                serviceCache_;

    std::shared_ptr<PBVolumeConfigMgr>              volumesConfigMgr_;
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

    /* Do a create in configdb */
    auto f = coordinationClient_->createIncludingAncestors(
        folly::sformat(configtree_constants::DATASPHERE_ROOT_PATH_FORMAT(), info.id),
        payload);
    f.get();

    /* update in memory state */
    {
        std::unique_lock<folly::SharedMutex> l(datasphereMutex_);
        datasphereTable_[info.id] = std::make_shared<DatasphereConfigMgr>(this, info);
    }

    CLog(INFO) << "Added new datasphere " << info;
}

void ConfigService::addService(const ServiceInfo &info)
{
    DatasphereConfigTable::iterator itr;
    {
        infra::SharedLock<folly::SharedMutex> l(datasphereMutex_);
        /* lookup datasphere */
        itr = datasphereTable_.find(info.dataSphereId);
        if (itr == datasphereTable_.end()) {
            CLog(WARNING) << " failed to add service.  datasphere:"
                << info.dataSphereId << " not found";
            throw StatusException(Status::STATUS_INVALID_DATASPHERE);
        }
    }
    /* add to datasphere */
    itr->second->addService(info);

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

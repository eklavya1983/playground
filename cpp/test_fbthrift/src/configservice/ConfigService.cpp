#include <configservice/ConfigService.h>
#include <folly/futures/Future.h>
#include <folly/Format.h>
#include <util/Log.h>
#include <infra/ZooKafkaClient.h>
#include <infra/Service.h>
#include <infra/Serializer.tcc>
#include <infra/gen/gen-cpp2/configtree_constants.h>
#include <infra/gen-ext/commontypes_ext.h>
#include <infra/gen/gen-cpp2/commontypes_types.tcc>
#include <infra/ConnectionCache.h>
#include <infra/MessageException.h>
#include <infra/gen/gen-cpp2/status_types.h>
#include <infra/LockHelper.tcc>
#include <infra/gen/gen-cpp2/ConfigApi.h>

#include <folly/io/async/EventBase.h>
#include <wangle/concurrent/IOThreadPoolExecutor.h>
#include <infra/gen-ext/KVBinaryData_ext.tcc>

namespace config {

using namespace infra;
using folly::via;

template<class ConfigServiceT, class ResourceT>
struct PBResourceSphereConfigMgr {
    PBResourceSphereConfigMgr(ConfigServiceT *parent,
                              const std::string &datasphereId,
                              const std::string &id,
                              int32_t replicaFactor,
                              folly::EventBase *eb)
    {
        parent_ = parent;
        datasphereId_ = datasphereId;
        id_ = id;
        replicaFactor_ = replicaFactor;
        eb_ = eb;
        nextAddRingId_ = 0;
        nextResourceId_ = 0;
        logContext_ = folly::sformat("{}:PBResourceSphereConfigMgr", parent_->getLogContext());
        resourcesTopic_ = folly::sformat(configtree_constants::TOPIC_PB_SPHERE_RESOURCES(),
                                         id_);
    }

    inline const std::string& getLogContext() const {
        return logContext_;
    }

    virtual VoidFuture addService(const ServiceInfo &info) {
        return via(eb_).then([this, info]() {
            pendingServices_.push_back(info.id);
            CLog(INFO) << "added service " << info;
            if (pendingServices_.size() >= static_cast<uint64_t>(replicaFactor_)) {
                // TODO(Rao): Support adding services when pending is >
                // replicaFactor_
                CHECK(pendingServices_.size() == static_cast<uint64_t>(replicaFactor_));
                RingInfo ring;
                ring.id = nextAddRingId_++;
                ring.memberIds = std::move(pendingServices_);
                pendingServices_.clear();
                auto f = addRing_(ring);
                f = f
                .onError([this, ring](const StatusException &e) {
                    // TODO(Rao): Handle this
                    CHECK(!"Failed to add.  Currenlty not handled");
                    CLog(INFO) << "failed to add ring: " << e.what();
                 });
                return f;
            }
            return folly::makeFuture();
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
                datasphereId_, id_, resource.id);
            auto payload = serializeToThriftJson<>(resource, getLogContext());

            CLog(INFO) << "add resource root at: " << resourceRoot;

            auto f = parent_->getCoordinationClient()->\
            createIncludingAncestors(resourceRoot, payload)
            .via(eb_)
            .then([this, resource](const std::string& data) {
                  resources_[resource.id] = resource;
                  CLog(INFO) << "added resource " << resource;
                  publishResourceInfo_(resource, 0);
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
                                       datasphereId_, id_, ring.id);
        auto payload = serializeToThriftJson<>(ring, getLogContext());

        CLog(INFO) << "add ring root at: " << ringRoot;

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

    virtual void publishResourceInfo_(const ResourceT &info, int64_t version)
    {

        /* Publish the resource information to resources topic */
        KVBinaryData kvb;
        setVersion(kvb, version);
        kvb.data = serializeToThriftJson<>(info, getLogContext());

        std::string payload = serializeToThriftJson<>(kvb, getLogContext());
        parent_->getCoordinationClient()->publishMessage(resourcesTopic_, payload);

        CLog(INFO) << "Published " << info
            << " resource information to ConfigDb version:" << version;
    }

    std::string                                 logContext_;
    ConfigServiceT                              *parent_;
    std::string                                 datasphereId_;
    std::string                                 id_;
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
    std::string                                 resourcesTopic_;
};

using PBVolumeConfigMgr = PBResourceSphereConfigMgr<ConfigService, VolumeInfo>;

struct DatasphereConfigMgr {
    DatasphereConfigMgr(ConfigService *parent, const DataSphereInfo &info)
        : parent_(parent),
        datasphereInfo_(info)
    {
    }
    virtual ~DatasphereConfigMgr() = default;

    void init()
    {
        volumesConfigMgr_ = std::make_shared<PBVolumeConfigMgr>(parent_,
                                                                datasphereInfo_.id,
                                                                "volumes",
                                                                /* TODO(Rao):
                                                                 * Don't hard
                                                                 * code */
                                                                3 /* replicafactor */,
                                                                parent_->getEventBaseFromPool());
    }

    const std::string& getLogContext() const { return parent_->getLogContext(); }

    // TODO(Rao): Add service is a multi step process, consider making it 1.
    // either a transaction or keep state to know up to which step is processed
    void addService(const ServiceInfo &info)
    {
        /* Serialize to TJson */
        std::string payload = serializeToThriftJson<ServiceInfo>(info, getLogContext());

        /* Update configdb */
        auto path = folly::sformat(configtree_constants::SERVICE_ROOT_PATH_FORMAT(),
                                   info.dataSphereId, info.id);
        auto f = parent_->getCoordinationClient()->createIncludingAncestors(
            path,
            payload);
        f.get();

        /* Update cache */
        {
            std::unique_lock<folly::SharedMutex> l(serviceCacheMutex_);
            serviceCache_[info.id] = info;
        }
        CLog(INFO) << "Added service at path:" << path << info;

        if (info.type == ServiceType::VOLUME_SERVER) {
            auto addFuture = volumesConfigMgr_->addService(info);
            try {
                (void) addFuture.get();
            } catch (const std::exception &e) {
                CLog(INFO) << "Failed to add service " << info << " to volumeconfigmgr";
            }
        }
    }

    VolumeInfo addVolume(const VolumeInfo &info)
    {
        auto f = volumesConfigMgr_->addResource(info);
        return f.get();
    }

 protected:
    ConfigService                                   *parent_;
    DataSphereInfo                                  datasphereInfo_;
    using ServiceInfoTable = std::unordered_map<std::string, ServiceInfo>;
    folly::SharedMutex                              serviceCacheMutex_;
    ServiceInfoTable                                serviceCache_;

    std::shared_ptr<PBVolumeConfigMgr>              volumesConfigMgr_;
};

struct ConfigApiHandler : ConfigApiSvIf {
    ConfigApiHandler(ConfigService *parent)
    {
        parent_ = parent;
    }
    void addService(std::unique_ptr< ::infra::ServiceInfo> info) override
    {
        parent_->addService(*info);
    }
    void addVolume(std::unique_ptr< ::infra::VolumeInfo> info) override
    {
        parent_->addVolume(*info);
    }

 protected:
    ConfigService                       *parent_;
};

ConfigService::ConfigService(const std::string &logContext,
                             const ServiceInfo &info,
                             const std::shared_ptr<CoordinationClient> &coordinationClient)
    : Service(logContext,
              info,
              std::make_shared<ConfigApiHandler>(this),
              coordinationClient)
{
}

ConfigService::~ConfigService()
{
    CLog(INFO) << "Exiting ConfigService";
}

void ConfigService::init()
{
    serviceEntryKey_ = configtree_constants::CONFIGSERVICE_ROOT();

    /* Init io threadpool */
    ioThreadpool_ = std::make_shared<wangle::IOThreadPoolExecutor>(2);

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

    /* Publish config service is up if datom is already configured */
    publishServiceInfomation_();

}

folly::EventBase* ConfigService::getEventBaseFromPool()
{
    return ioThreadpool_->getEventBase();
}

void ConfigService::createDatom()
{
    /* Do a create */
    auto f = coordinationClient_->create(getServiceEntryKey(), "");
    CLog(INFO) << "Created config service root at:" << getServiceEntryKey();

    publishServiceInfomation_();
}

void ConfigService::addDataSphere(const DataSphereInfo &info)
{
    /* Serialize to TJson */
    std::string payload = serializeToThriftJson<DataSphereInfo>(info, getLogContext());

    /* Do a create in configdb */
    auto f = coordinationClient_->createIncludingAncestors(
        folly::sformat(configtree_constants::DATASPHERE_ROOT_PATH_FORMAT(), info.id),
        payload);
    f.get();

    /* update in memory state */
    {
        std::unique_lock<folly::SharedMutex> l(datasphereMutex_);
        auto datasphere = std::make_shared<DatasphereConfigMgr>(this, info);
        datasphere->init();
        datasphereTable_[info.id] = datasphere;
    }

    CLog(INFO) << "Added new datasphere " << info;
}

void ConfigService::addService(const ServiceInfo &info)
{
    auto itr = getDatasphereOrThrow_(info.dataSphereId);
    /* add to datasphere */
    itr->second->addService(info);

}

void ConfigService::startVolumeCluster()
{
}

void ConfigService::startDataCluster()
{
}

VolumeInfo ConfigService::addVolume(const VolumeInfo &info)
{
    auto itr = getDatasphereOrThrow_(info.datasphereId);
    return itr->second->addVolume(info);
}

void ConfigService::ensureDatasphereMembership_()
{
    /* No need to check for memebership for config service */
}

ConfigService::DatasphereConfigTable::iterator
ConfigService::getDatasphereOrThrow_(const std::string &id)
{
    DatasphereConfigTable::iterator itr;
    {
        infra::SharedLock<folly::SharedMutex> l(datasphereMutex_);
        /* lookup datasphere */
        itr = datasphereTable_.find(id);
        if (itr == datasphereTable_.end()) {
            CLog(WARNING) << " failed to add service.  datasphere:"
                << id << " not found";
            throw StatusException(Status::STATUS_INVALID_DATASPHERE);
        }
    }
    return itr;
}

}  // namespace config

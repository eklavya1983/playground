#include <volumeserver/VolumeServer.h>
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
#include <wangle/concurrent/IOThreadPoolExecutor.h>
#include <infra/gen/configtree_constants.h>
#include <infra/gen-ext/KVBinaryData_ext.tcc>

namespace volumeserver {

template<class ParentT>
struct PBResourceReplica {
    PBResourceReplica(ParentT *parent, const KVBinaryData &kvb)
        : parent_(parent)
    {
    }
    virtual void init()
    {
    }
 protected:
    ParentT                 *parent_;
};

// TODO(Rao):
// 1. Subscribe to events for volume table
// 2. Pulling rings and adding resources based based on rings
template <class ParentT, class ResourceT>
struct PBResourceMgr {
    using ResourceTable = std::unordered_map<int64_t, std::shared_ptr<ResourceT>>;

    PBResourceMgr(ParentT* parent, const std::string &id)
    {
        parent_ = parent;
        id_ = id;
        logContext_ = folly::sformat("{}:PBResourceMgr:{}", parent->getLogContext(), id_);
    }
    virtual ~PBResourceMgr() = default;
    virtual void init()
    {
        pullResourceConfigTable_();
    }
    virtual void addResource()
    {
        CHECK(!"Unimplemented");
    }
    virtual std::shared_ptr<ResourceT> getResource()
    {
        CHECK(!"Unimplemented");
        return nullptr;
    }
    const std::string& getLogContext() const {
        return logContext_;
    }

 protected:
    virtual void pullResourceConfigTable_()
    {
        CLog(INFO) << "Pulling resouces from configdb";

        /* Pull resource table from configdb */
        auto resourcesRoot = folly::sformat(
            configtree_constants::PB_SPHERE_RESOURCES_ROOT_PATH_FORMAT(),
            parent_->getDatasphereId(),
            id_);
        auto kvbVector = parent_->getCoordinationClient()->getChildrenSync(resourcesRoot);

        std::unique_lock<folly::SharedMutex> l(resourceTableMutex_);
        for (const auto &kvb : kvbVector) {
            auto id = folly::to<int64_t>(getId(kvb));
            auto resource = std::make_shared<ResourceT>(parent_, kvb);
            resourceTable_[id] = resource;
            resource->init();
            CLog(INFO) << "added resouce:" << id;
        }

        CLog(INFO) << "Pulling resouces from configdb";
    }

    ParentT                                             *parent_;
    std::string                                         logContext_;
    std::string                                         id_;
    folly::SharedMutex                                  resourceTableMutex_;
    ResourceTable                                       resourceTable_;
};

void VolumeServer::init()
{
    Service::init();

    ioThreadpool_ = std::make_shared<wangle::IOThreadPoolExecutor>(2);
    replicaMgr_ = std::make_shared<VolumeReplicaMgr>(this, "volumes");
    replicaMgr_->init();
}

folly::EventBase* VolumeServer::getEventBaseFromPool()
{
    return ioThreadpool_->getEventBase();
}

}  // namespace volumeserver 

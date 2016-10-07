#include <exception>
#include <zookeeper.h>
#include <testlib/DatomBringupHelper.h>
#include <folly/futures/Future.h>
#include <folly/Format.h>
#include <infra/ZooKafkaClient.h>
#include <infra/Service.h>
#include <infra/Serializer.tcc>
#include <infra/gen/configtree_constants.h>
#include <infra/gen-ext/commontypes_ext.h>
#include <infra/ConnectionCache.h>
#include <infra/StatusException.h>
#include <infra/gen/status_types.h>

DEFINE_string(toolsdir, "", "tools directory");

namespace testlib {
using namespace infra;

struct ConfigService : Service {
    using Service::Service;

    virtual ~ConfigService() {
        CLog(INFO) << "Exiting ConfigService";
    }

    void init() override {
        serviceEntryKey_ = g_configtree_constants.CONFIGSERVICE_ROOT;

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

    /* NOTE: Below methods don't do checks for existence.  It is intentional as
     * this code is just testing.  It is up to the user to ensure this isn't
     * exploited
     */
    void createDatom() {
        /* Do a create */
        auto f = coordinationClient_->create(g_configtree_constants.DATOM_ROOT, "");
        f.get();
        CLog(INFO) << "Created datom root at:" << g_configtree_constants.DATOM_ROOT;
        f = coordinationClient_->create(getServiceEntryKey(), "");
        CLog(INFO) << "Created config service root at:" << getServiceEntryKey();

        publishServiceInfomation_();
    }

    void addDataSphere(const DataSphereInfo &info)
    {
        /* Serialize to TJson */
        std::string payload;
        serializeToThriftJson<DataSphereInfo>(info, payload, getLogContext());

        /* Do a create */
        auto f = coordinationClient_->createIncludingAncestors(
            folly::sformat(g_configtree_constants.DATASPHERE_ROOT_PATH_FORMAT, info.id),
            payload);
        f.get();
        CLog(INFO) << "Added new datasphere id:" << info.id;
    }

    void addService(const ServiceInfo &info)
    {
        /* Serialize to TJson */
        std::string payload;
        serializeToThriftJson<ServiceInfo>(info, payload, getLogContext());

        /* Do a Put */
        auto f = coordinationClient_->createIncludingAncestors(
            folly::sformat(g_configtree_constants.SERVICE_ROOT_PATH_FORMAT,
                           info.dataSphereId, info.id),
            payload);
        f.get();
        CLog(INFO) << "Added service " << info;
    }

    void startVolumeCluster()
    {
    }

    void startDataCluster()
    {
    }

 protected:
    void ensureDatasphereMembership_() override {
        /* No need to check for memebership for config service */
    }

    bool datomConfigured_ {false};
};

DatomBringupHelper::DatomBringupHelper()
{
    if (FLAGS_toolsdir.empty()) {
        throw std::runtime_error("toolsdir flag isn't specified");
    }
    taskScript_ = folly::sformat("{}/task.sh", FLAGS_toolsdir);
}

void DatomBringupHelper::cleanStartDatom()
{
    /* Bringup zookeeper and kafka */
    std::system(folly::sformat("{} cleanstartdatom", taskScript_).c_str());

    /* Bringup  config service */
    auto zkClient = std::make_shared<ZooKafkaClient>("ConfigService",
                                                     "localhost:2181",
                                                     "ConfigService");
    configService_ = std::make_shared<ConfigService>("ConfigService",
                                                     ServiceInfo(),
                                                     false,
                                                     zkClient);
    configService_->init();

    /* Create datom namespace */
    configService_->createDatom();
}

void DatomBringupHelper::cleanStopDatom()
{
    configService_.reset();
    std::system(folly::sformat("{} cleanstopdatom", taskScript_).c_str());
}

void DatomBringupHelper::shutdownDatom()
{
    configService_.reset();
    std::system(folly::sformat("{} stopdatom", taskScript_).c_str());
}

void DatomBringupHelper::addDataSphere(const std::string &dataSphereId)
{
    DataSphereInfo info;
    info.id = dataSphereId;
    configService_->addDataSphere(info);
}

void DatomBringupHelper::addService(const std::string &dataSphereId,
                                    const std::string &nodeId,
                                    const std::string &serviceId,
                                    const std::string &ip,
                                    const int port)
{
    ServiceInfo info;
    info.id = serviceId;
    info.nodeId = nodeId;
    info.dataSphereId = dataSphereId;
    info.ip = ip;
    info.port = port;
    configService_->addService(info);
}

ScopedDatom::ScopedDatom(DatomBringupHelper& d)
    : datom_(d)
{
    datom_.cleanStartDatom();
}

ScopedDatom::~ScopedDatom()
{
    datom_.cleanStopDatom();
}


}  // namespace testlib 

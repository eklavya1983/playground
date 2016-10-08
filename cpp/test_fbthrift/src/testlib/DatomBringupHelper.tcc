#pragma once

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

namespace testlib {
using namespace infra;

template <class ConfigServiceT>
DatomBringupHelper<ConfigServiceT>::DatomBringupHelper()
{
}

template <class ConfigServiceT>
void DatomBringupHelper<ConfigServiceT>::cleanStartDatom()
{
    /* Bringup zookeeper and kafka */
    KafkaRunner_.cleanstart();

    /* Bringup  config service */
    auto zkClient = std::make_shared<ZooKafkaClient>("ConfigService",
                                                     "localhost:2181/datom",
                                                     "ConfigService");
    configService_ = std::make_shared<ConfigServiceT>("ConfigService",
                                                      ServiceInfo(),
                                                      false,
                                                      zkClient);
    configService_->init();

    /* Create datom namespace */
    configService_->createDatom();
}

template <class ConfigServiceT>
void DatomBringupHelper<ConfigServiceT>::cleanStopDatom()
{
    configService_.reset();
    KafkaRunner_.cleanstop();
}

template <class ConfigServiceT>
void DatomBringupHelper<ConfigServiceT>::shutdownDatom()
{
    configService_.reset();
    KafkaRunner_.stop();
}

template <class ConfigServiceT>
void DatomBringupHelper<ConfigServiceT>::addDataSphere(const std::string &dataSphereId)
{
    DataSphereInfo info;
    info.id = dataSphereId;
    configService_->addDataSphere(info);
}

template <class ConfigServiceT>
void DatomBringupHelper<ConfigServiceT>::addService(const std::string &dataSphereId,
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

template <class ConfigServiceT>
ScopedDatom<ConfigServiceT>::ScopedDatom(DatomBringupHelper<ConfigServiceT>& d)
    : datom_(d)
{
    datom_.cleanStartDatom();
}

template <class ConfigServiceT>
ScopedDatom<ConfigServiceT>::~ScopedDatom()
{
    datom_.cleanStopDatom();
}


}  // namespace testlib 

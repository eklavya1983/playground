#pragma once
#include <memory>

namespace apache {
namespace thrift {
class ThriftServer;
namespace util {
class ScopedServerThread;
}  // namespace util
}  // namespace thrift
}  // namespace apache

namespace infra {

namespace cpp2 {
struct ServiceApiSvIf;
}

/**
 * @brief Default server for service
 */
struct ServiceServer {
    ServiceServer(const std::string &logContext,
                  const std::string &ip,
                  int port,
                  const std::shared_ptr<cpp2::ServiceApiSvIf> &handler);
    virtual ~ServiceServer() = default;
    ServiceServer(const ServiceServer&) = delete;
    void operator=(ServiceServer const &) = delete;

    virtual void start();
    virtual void stop();

 protected:
    std::string                                             logContext_;
    std::string                                             ip_;
    int                                                     port_;
    std::shared_ptr<cpp2::ServiceApiSvIf>                   handler_;
    std::shared_ptr<apache::thrift::ThriftServer>           server_;
    apache::thrift::util::ScopedServerThread                *serverThread_ {nullptr};
};

}  // namespace infra

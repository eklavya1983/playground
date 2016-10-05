#include <string>

namespace infra {
struct CoordinationClient;
struct ConnectionCache;

struct ModuleProvider {
    virtual std::string getDatasphereId() const { return ""; }
    virtual std::string getNodeId() const { return ""; }
    virtual std::string getServiceId() const { return ""; }
    virtual CoordinationClient* getCoordinationClient() const { return nullptr; }
    virtual ConnectionCache*    getConnectionCache() const { return nullptr; }
};

}  // namespace infra

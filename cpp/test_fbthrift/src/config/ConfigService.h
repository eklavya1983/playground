#include <actor/gen-cpp2/Service_constants.h>
#include <actor/ActorSystem.hpp>

namespace config {
using namespace actor;
using namespace actor::cpp2;

struct ConfigService : ActorSystem {
    ConfigService(const std::string &configIp,
                  int configPort);
    void init() override;
    static const char* className() {return "ConfigService";} 

protected:
    virtual void initBehaviors_() override;
    void handleRegisterMsg_();

    ActorSystemId  nextSystemId_;

};

}  // namespace config

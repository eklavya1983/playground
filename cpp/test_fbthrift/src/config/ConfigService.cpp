#include <actor/gen-cpp2/Service_constants.h>
#include <actor/ActorSystem.hpp>

DEFINE_int32(port, 8000, "Config port");

namespace config {
using namespace actor;
using namespace actor::cpp2;

struct ConfigServiceActor : ActorSystem {
    ConfigServiceActor(int myPort,
                const std::string &configIp,
                int configPort)
    : ActorSystem(myPort, configIp, configPort)
    {
    }

protected:
    virtual void initBehavior_() override {
        ActorSystem::initBehavior_();

        functionalBehavior_ += {
            on(ConfigMsgTypes::RegisterActorSystemMsg) >> [this](ActorMsg &&msg) {
                auto &payloadPtr = msgPayloadPtr<RegisterActorSystem>(msg);
                updateActorRegistry_(payloadPtr->systemInfo);
                // TODO: Broadcast
                auto resp = std::make_shared<RegisterActorSystemResp>();
                reply(msg, ConfigMsgTypes::RegisterActorSystemRespMsg, resp);
            }
        };
    }

    void handleRegisterServiceMsg_(ActorMsg &&msg) {
        std::cout << "handleRegisterServiceMsg called";
        assert(!"Unimplemented");
    }

    Behavior behavior_;
};

}  // namespace config


int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);

    /* Start actor system */
    actor::ActorSystem system(FLAGS_port,"localhost", 0);
    auto configActor = system.spawnRootActor<config::ConfigServiceActor>();
    system.init();
    system.loop();
}

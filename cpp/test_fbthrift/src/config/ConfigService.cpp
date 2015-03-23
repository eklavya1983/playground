#include <actor/gen-cpp2/Service_constants.h>
#include <util/Log.h>
#include <actor/Error.h>
#include <actor/ActorSystem.hpp>

DEFINE_int32(port, 8000, "Config port");

namespace config {
using namespace actor;
using namespace actor::cpp2;

struct ConfigServiceActor : ActorSystem {
    ConfigServiceActor(int myPort,
                const std::string &configIp,
                int configPort)
    : ActorSystem("config", myPort, configIp, configPort)
    {
        nextSystemId_ = configActorId().systemId + 1;
    }

protected:
    virtual void initBehavior_() override {
        ActorSystem::initBehavior_();

        functionalBehavior_ += {
            on(Register) >> [this]() {
                handleRegisterMsg_();
            }
        };
    }

    void handleRegisterMsg_() {
        auto &systemInfo = msgPayload<Register>().systemInfo;

        ALog(INFO) << "Receieved register message from: " << systemInfo;

        if (systemInfo.id == invalidActorId()) {
            /* First time registration. Assign an id */
            systemInfo.id = ActorId(apache::thrift::FRAGILE, nextSystemId_, LOCALID_START);
            nextSystemId_++;
        }
        auto resp = std::make_shared<RegisterResp>();
        resp->error = static_cast<int32_t>(updateActorRegistry_(systemInfo, true));
        if (resp->error == 0) {
            resp->id = systemInfo.id;
        } else {
            nextSystemId_--;
        }
        reply<RegisterResp>(resp);
        // TODO: Broadcast
    }

    Behavior behavior_;
    ActorSystemId  nextSystemId_;

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

#include <actor/gen-cpp2/ConfigApi.h>
#include <actor/gen-cpp2/Service_constants.h>
#include <actor/ActorSystem.hpp>

DEFINE_int32(port, 8000, "Config port");

namespace config {
using namespace actor;
using namespace actor::cpp2;

struct ConfigServiceActor : NotificationQueueActor {
    explicit ConfigServiceActor(folly::EventBase *eventBase)
    : NotificationQueueActor(eventBase)
    {
        functionalBehavior_ += {
            on(ConfigMsgTypes::RegisterServiceMsg) >> [this](ActorMsg &&msg) {
                handleRegisterServiceMsg(std::move(msg));
            }
        };
    }

    void handleRegisterServiceMsg(ActorMsg &&msg) {
        std::cout << "handleRegisterServiceMsg called";
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

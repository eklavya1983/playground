#include <actor/gen-cpp2/ConfigApi.h>
#include <actor/gen-cpp2/Service_constants.h>
#include <actor/ActorSystem.hpp>

DEFINE_int32(port, 8100, "Config port");

namespace data {
using namespace actor;
using namespace actor::cpp2;

struct DataAccessServiceActor : NotificationQueueActor {
    explicit DataAccessServiceActor(folly::EventBase *eventBase)
    : NotificationQueueActor(eventBase)
    {
        functionalBehavior_ += {
            on(DataAcessMsgTypes::AddVolumeMsg) >> [this](ActorMsg &&msg) {
                handleAddVolumeMsg(std::move(msg));
            }
        };
    }
    
    void handleInitMsg(ActorMsg &&msg) {
    }

    void handleAddVolumeMsg(ActorMsg &&msg) {
    }

};

}  // namespace data 


int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);

    /* Start actor system */
    actor::ActorSystem system(FLAGS_port,"localhost", 0);
    auto configActor = system.spawnActor<data::DataAccessServiceActor>();
    system.init();
    system.loop();
}

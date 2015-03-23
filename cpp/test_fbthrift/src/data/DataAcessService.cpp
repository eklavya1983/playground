#include <actor/gen-cpp2/Service_constants.h>
#include <actor/ActorSystem.hpp>

DEFINE_int32(port, 8100, "Config port");

namespace data {
using namespace actor;
using namespace actor::cpp2;

struct DataAccessServiceActor : ActorSystem {
    explicit DataAccessServiceActor(int myPort,
                const std::string &configIp,
                int configPort)
    : ActorSystem(myPort, configIp, configPort)
    {
    }

 protected:
    virtual void initBehavior_() override {
        ActorSystem::initBehavior_();

        functionalBehavior_ += {
            on(AddVolume) >> [this]() {
                handleAddVolumeMsg_();
            }
        };
    }
    
    void handleInitMsg_() {
    }

    void handleAddVolumeMsg_() {
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

#include <util/Log.h>
#include <actor/ActorSystem.hpp>
#include <data/DataAcessService.h>
#include <util/TypeMappings.h>

DEFINE_int32(dataPort, 9000, "data port");

namespace bhoomi {
using namespace actor;
using namespace actor::cpp2;


DataAccessService::DataAccessService(int myPort,
                                     const std::string &configIp,
                                     int configPort)
    : ActorSystem(invalidActorId(), "data", nullptr, myPort, configIp, configPort)
{
}

void DataAccessService::initBehaviors_() {
    ActorSystem::initBehaviors_();

    functionalBehavior_ += {
        on(GroupAddVolume) >> [this]() {
        }
    };
}

}  // bhoomi

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    /* Start actor system */
    bhoomi::initActorMsgMappings();
    actor::ActorSystemPtr system(new bhoomi::DataAccessService(FLAGS_dataPort, "127.0.0.1", 8000));
    CHECK(system == system->getPtr());
    system->init();
    system->loop();
}

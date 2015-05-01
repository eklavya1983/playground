#include <util/Log.h>
#include <actor/Actor.hpp>
#include <config/ConfigService.h>

DEFINE_int32(port, 8000, "Config port");

namespace config {
using namespace actor;
using namespace actor::cpp2;

ConfigService::ConfigService(const std::string &configIp,
                             int configPort)
    : ActorSystem("config", configPort, configIp, configPort)
{
    setId(configActorId());
    nextSystemId_ = configActorId().systemId + 1;
}

void ConfigService::init() {
    /* We need to add ourself in to actorTbl so messages can be routed */
    actorTbl_.insert(std::make_pair(toInt64(myId_), getPtr()));
    ActorSystem::init();
}

void ConfigService::initBehaviors_() {
    ActorSystem::initBehaviors_();

    initBehavior_ += {
        on(Init) >> [this]() {
            changeBehavior(&functionalBehavior_);
        }
    };
    functionalBehavior_ += {
        on(Register) >> [this]() {
            handleRegisterMsg_();
        }
    };
}

void ConfigService::handleRegisterMsg_() {
    auto &systemInfo = payload<Register>().systemInfo;

    /* Assign id to first time registring system */
    if (systemInfo.id == invalidActorId()) {
        AVLog(LCONFIG) << "First time register message from: " << systemInfo;
        /* First time registration. Assign an id */
        systemInfo.id = ActorId(apache::thrift::FRAGILE, nextSystemId_, LOCALID_START);
        nextSystemId_++;
    } else {
        AVLog(LCONFIG) << "First time register message from: " << systemInfo;
    }

    /* Accept by registering/updaging the system information */
    auto resp = std::make_shared<RegisterResp>();
    resp->error = static_cast<int32_t>(updateActorRegistry_(systemInfo, true));
    if (resp->error == 0) {
        resp->id = systemInfo.id;
    } else {
        CHECK(false) << "Failed to register";
    }

    auto replyMsg = makeActorMsg<RegisterResp>(myId_, systemInfo.id,  std::move(resp)); 
    system_->routeToActor(std::move(replyMsg));
}

}  // namespace config


int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    /* Start actor system */
    actor::initActorMsgMappings();
    actor::ActorSystemPtr system(new config::ConfigService("127.0.0.1", 8000));
    CHECK(system == system->getPtr());
    system->init();
    system->loop();
}

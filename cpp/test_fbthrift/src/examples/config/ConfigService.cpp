#include <util/Log.h>
#include <folly/futures/Future.h>
#include <actor/gen-cpp2/ConfigApi.h>
#include <actor/Actor.hpp>
#include <config/ConfigService.h>
#include <util/TypeMappings.h>

DEFINE_int32(port, 8000, "Config port");

namespace bhoomi {
using namespace actor;
using namespace actor::cpp2;

struct ConfigHandler : virtual ::actor::cpp2::ConfigApiSvIf, ServiceHandler {
    using ServiceHandler::ServiceHandler;

    virtual void registerActorSystem(::actor::cpp2::ActorId& _return,
                                     std::unique_ptr<::actor::cpp2::ActorInfo> info) override {
        auto payload = std::make_shared<Register>();
        payload->info = std::move(info);
        auto f = payload->promise.getFuture();
        system_->send(makeActorMsg<Register>(std::move(payload)));
        f.wait();
        _return = f.value();
    }
};

ConfigService::ConfigService(const std::string &configIp,
                             int configPort)
    : ActorSystem(configActorId(), "config",
                  std::unique_ptr<ConfigHandler>(new ConfigHandler(this)),
                  configPort, configIp, configPort)
{
    nextSystemId_ = configActorId().systemId + 1;
}

void ConfigService::init() {
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
    auto &req = payload<Register>();
    auto &systemInfo = *(req.info);

    /* Assign id to first time registring system */
    if (systemInfo.id == invalidActorId()) {
        AVLog(LCONFIG) << "First time register message from: " << systemInfo;
        /* First time registration. Assign an id */
        systemInfo.id = ActorId(apache::thrift::FRAGILE, nextSystemId_, LOCALID_START);
        nextSystemId_++;
    } else {
        AVLog(LCONFIG) << "Existing actorsystem register message from: " << systemInfo;
    }

    /* Accept by registering/updaging the system information */
    auto ret = static_cast<int32_t>(updateActorRegistry_(systemInfo, true));
    if (ret == 0) {
        req.promise.setValue(systemInfo.id);
    } else {
        req.promise.setException(RegisterException());
    }
}

void initMappings() {
    bhoomi::initActorMsgMappings();
    ADD_MSGMAPPING(Register,                       7);
}

}  // namespace bhoomi 


int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    bhoomi::initMappings();
    /* Start actor system */
    actor::ActorSystemPtr system(new bhoomi::ConfigService("127.0.0.1", 8000));
    CHECK(system == system->getPtr());
    system->init();
    system->loop();
}

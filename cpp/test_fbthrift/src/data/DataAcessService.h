#pragma once

#include <actor/ActorSystem.h>

namespace data {

struct DataAccessService : actor::ActorSystem {
    explicit DataAccessService(int myPort,
                const std::string &configIp,
                int configPort);

 protected:
    virtual void initBehaviors_() override;
};

}  // namespace data 

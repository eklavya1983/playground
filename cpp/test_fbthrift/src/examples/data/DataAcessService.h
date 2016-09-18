#pragma once

#include <actor/ActorSystem.h>

namespace bhoomi {

struct DataAccessService : actor::ActorSystem {
    explicit DataAccessService(int myPort,
                const std::string &configIp,
                int configPort);

 protected:
    virtual void initBehaviors_() override;
};

}  // namespace data 

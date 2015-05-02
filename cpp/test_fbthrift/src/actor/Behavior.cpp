#include <util/Log.h>
#include <actor/Behavior.h>

namespace actor {

std::ostream& operator<<(std::ostream &stream, const BehaviorKey &k) {
    stream << actorMsgName(k.typeId) << ":" << k.typeId;
    if (k.direction == MSGDIRECTION_RESPONSE) {
        stream << ":resp";
    }
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b) {
    stream << "[" << b.behaviorKey << "] -> [" << b.handlerName << "]";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Behavior& b)
{
    stream << "Behavior Name: " << b.name_ << "\n";
    for (auto &itr : b.handlers_) {
        stream << itr.second << "\n";
    }
    return stream;
}

void Behavior::handle(const BehaviorKey& key) {
    auto handlerItr = handlers_.find(key);
    if (handlerItr == handlers_.end()) {
        if (key.direction == MSGDIRECTION_RESPONSE) {
            handlerItr = handlers_.find({MSGDIRECTION_RESPONSE, ActorMsgTypeEnum<Other>::typeId});
        }
        if (handlerItr == handlers_.end()) {
            handlerItr = handlers_.find({MSGDIRECTION_NORMAL, ActorMsgTypeEnum<Other>::typeId});
        }
        if (handlerItr == handlers_.end()) {
            CHECK(false) << "TODO: Implement";
            // TODO: Determine behavior here
            return;
        }
    }
#if 0
    VLOG(BEHAVIOR) << "handle() type: " << typeId << "->"
        << handlerItr->second.file << ":" << handlerItr->second.lineno;
#endif
    (handlerItr->second.handler)();
}
}  // namespace actor

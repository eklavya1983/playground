#include <util/Log.h>
#include <actor/Behavior.h>

namespace actor {

std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b) {
    stream << "[" << actorMsgName(b.typeId) << ":" << b.typeId << "] -> ["
        << b.handlerName << "]";
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

void Behavior::handle(const ActorMsgTypeId &typeId) {
    auto handlerItr = handlers_.find(typeId);
    if (handlerItr == handlers_.end()) {
        handlerItr = handlers_.find(ActorMsgTypeEnum<Other>::typeId);
        if (handlerItr == handlers_.end()) {
            assert(!"TODO: Implement"); 
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

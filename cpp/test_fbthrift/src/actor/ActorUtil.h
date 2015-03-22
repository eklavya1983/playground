#pragma once
#include <sstream>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

using namespace cpp2;

inline int64_t toInt64(const ActorId &id) {
    ActorId &id2 = const_cast<ActorId&>(id);
    return *(reinterpret_cast<int64_t*>(&(id2.systemId)));
}

template <class T>
inline T& operator << (T &stream, const ActorInfo &info) {
    stream << "ip: " << info.ip << " port: " << info.port;
}

template <class T>
inline T& operator << (T &stream, const ActorId &id) {
    stream << "[" << id.systemId << ":" << id.localId << "]";
}

}  // namespace actor

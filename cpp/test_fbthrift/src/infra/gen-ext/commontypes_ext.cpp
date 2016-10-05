#include <infra/gen/commontypes_types.h>

namespace infra {

std::ostream& operator << (std::ostream& out, const ServiceInfo &info)
{
    out << " [" << info.dataSphereId << ":" << info.id << "]"
        << " ip:" << info.ip << " port:" << info.port;
    return out;
}

}  // namespace infra

#pragma once
#include <ostream>

namespace infra {
struct ServiceInfo;

std::ostream& operator << (std::ostream& out, const ServiceInfo &info);

}  // namespace infra

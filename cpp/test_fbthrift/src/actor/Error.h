#pragma once
#include <actor/gen-cpp2/Service_types.h>
namespace actor {
using namespace cpp2;
inline bool ok(const Error &e) {
    return e == Error::ERR_OK;
}
}  // namespace actor

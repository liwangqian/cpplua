#ifndef CPPLUA_UPVALUE_H
#define CPPLUA_UPVALUE_H

#include "def.h"

CPPLUA_NS_BEGIN
namespace __detail {

class upvalue {

};

} // namespace __detail

// exports
using upvalue_t = __detail::upvalue;

CPPLUA_NS_END

#endif

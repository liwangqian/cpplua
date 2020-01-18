#ifndef CPPLUA_VM_ERROR_H
#define CPPLUA_VM_ERROR_H

#include "def.h"

CPPLUA_NS_BEGIN

enum class error_t : uint8_t {
    E_OK = 0,
    E_NOT_EXSIT,
    E_OPEN,
    E_FORMAT,
    E_VERSION,
    E_LENGTH,
    E_MEMORY,
    E_PARAM,
};

CPPLUA_NS_END

#endif

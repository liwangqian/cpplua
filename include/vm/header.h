#ifndef CPPLUA_VM_HEADER_H
#define CPPLUA_VM_HEADER_H

#include "def.h"
#include "vm/error.h"

CPPLUA_NS_BEGIN
namespace __detail{

struct header {
    uint32_t signature;
    uint8_t  version;
    uint8_t  format;
    uint8_t  luac_data[6];
    uint8_t  cint_size;
    uint8_t  csizet_size;
    uint8_t  instruction_size;
    uint8_t  lua_integer_size;
    uint8_t  lua_number_size;
    uint8_t  luac_int;
    uint8_t  luac_num;
};

} // namespace __detail

using header_t = __detail::header;

CPPLUA_NS_END

#endif

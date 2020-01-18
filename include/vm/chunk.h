#ifndef CPPLUA_CHUNK_H
#define CPPLUA_CHUNK_H

#include "def.h"
#include "header.h"

CPPLUA_NS_BEGIN
namespace __detail {

constexpr uint32_t LUAC_SIGNATURE = 0x1b4c7561;
constexpr uint8_t  LUAC_DTAT[] = {0x19, 0x93, '\r', '\n', 0x1a, '\n'};

class prototype_t;

class chunk {
public:
    chunk();
    ~chunk();

private:
    header_t header;
    uint8_t  num_upvalues;
    prototype_t *fmain;
};

chunk_t new_chunk();

} // namespace __detail

// exports
using chunk_t = __detail::chunk_t;

CPPLUA_NS_END

#endif

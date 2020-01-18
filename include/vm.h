#ifndef CPPLUA_VM_H
#define CPPLUA_VM_H

#include "def.h"
#include "vm/chunk.h"
#include "vm/constant.h"
#include "vm/instruction.h"
#include "vm/prototype.h"
#include "vm/upvalue.h"

CPPLUA_NS_BEGIN
namespace __detail {

class binary_chunk;
class binary_header;

class vm
{
public:
    vm(/* args */);
    ~vm();

private:
    /* data */
};

} // namespace __detail

using lvm_t = __detail::vm;

CPPLUA_NS_END

#endif

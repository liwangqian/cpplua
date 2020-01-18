#ifndef CPPLUA_PROTOTYPE_H
#define CPPLUA_PROTOTYPE_H

#include "def.h"
#include "range.h"
#include "upvalue.h"
#include "constant.h"
#include <vector>

CPPLUA_NS_BEGIN
namespace __detail{

class prototype
{
public:
    prototype() {};
    ~prototype() {};

private:
    using code_table_t = std::vector<uint32_t>;
    using constant_table_t = std::vector<constant_t>;

    string_t source_file;
    range_t range;
    uint8_t num_params;
    uint8_t max_stack_size;
    bool is_vararg;
    code_table_t codes;
    constant_table_t constants;
};

} // namespace __detail

using prototype_t = __detail::prototype;

CPPLUA_NS_END

#endif

#ifndef CPPLUA_CONSTANT_H
#define CPPLUA_CONSTANT_H

#include "def.h"
#include <cassert>

CPPLUA_NS_BEGIN
namespace __detail {

class constant
{
public:
    constant()
    : type{constant_type::NIL}
    {
    }

    ~constant() = default;

    bool as_bool() const
    {
        assert(type == constant_type::BOOLEAN);
        return value.boolean;
    }

    float as_number() const
    {
        assert(type == constant_type::NUMBER);
        return value.number;
    }

    uint64_t as_integer() const
    {
        assert(type == constant_type::INTEGER);
        return value.integer;
    }

    string_t as_string() const
    {
        assert(type == constant_type::SHORT_STR || type == constant_type::LONG_STR);
        return value.string;
    }

private:
    enum class constant_type : uint8_t {
        NIL       = 0x00,
        BOOLEAN   = 0x01,
        NUMBER    = 0x03,
        INTEGER   = 0x13,
        SHORT_STR = 0x04,
        LONG_STR  = 0x14,
    };

    constant_type type;
    union value_t
    {
        bool     boolean;
        float    number;
        uint64_t integer;
        string_t string;

        value_t() {}
        ~value_t() {}
    } value;
};

using constant_t = __detail::constant;

} // namespace __detail

CPPLUA_NS_END

#endif


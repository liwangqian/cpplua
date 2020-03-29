#ifndef CPPLUA_FEATURE_H
#define CPPLUA_FEATURE_H

#include "def.h"
#include "cstring"

CPPLUA_NS_BEGIN
namespace __detail {

#define BIT(n) (1U << n)

enum feature_bits {
    lables = BIT(0U),
    empty_statement = BIT(1U),
    hex_escapes = BIT(2U),
    skip_whitespace_escape = BIT(3U),
    strict_escapes = BIT(4U),
    unicode_escapes = BIT(5U),
    bitwise_operators = BIT(6U),
    integer_division = BIT(7U),
    contextual_goto = BIT(8U),
};

class feature {
public:
    bool supports(feature_bits features)
    {
        return (m_feature & features) == features;
    }

    ~feature() = default;

    feature(unsigned int val) : m_feature(val)
    {}

private:
    unsigned int m_feature;
};

} // namespace __detail

// exports
using feature_enum_t = __detail::feature_bits;
using feature_t = __detail::feature;

static inline feature_t make_feature(const char *lua_version)
{
    if (strcmp(lua_version, "5.1") == 0) {
        return {0};
    }

    if (strcmp(lua_version, "5.2") == 0) {
        return {
            feature_enum_t::lables |
            feature_enum_t::empty_statement |
            feature_enum_t::hex_escapes |
            feature_enum_t::skip_whitespace_escape |
            feature_enum_t::strict_escapes};
    }

    if (strcmp(lua_version, "5.3") == 0) {
        return {
            feature_enum_t::lables |
            feature_enum_t::empty_statement |
            feature_enum_t::hex_escapes |
            feature_enum_t::skip_whitespace_escape |
            feature_enum_t::strict_escapes |
            feature_enum_t::unicode_escapes |
            feature_enum_t::bitwise_operators |
            feature_enum_t::integer_division};
    }

    if (strcmp(lua_version, "LuaJit") == 0) {
        return {
            feature_enum_t::lables |
            feature_enum_t::contextual_goto |
            feature_enum_t::hex_escapes |
            feature_enum_t::skip_whitespace_escape |
            feature_enum_t::strict_escapes |
            feature_enum_t::unicode_escapes};
    }

    return {0};
}

CPPLUA_NS_END

#endif

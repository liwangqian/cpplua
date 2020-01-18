#ifndef CPPLUA_VM_ENDIAN_H
#define CPPLUA_VM_ENDIAN_H

#include "def.h"

CPPLUA_NS_BEGIN
namespace __detail {

static constexpr bool host_is_little_endian()
{
    union {
        uint32_t v;
        uint8_t  c[sizeof(uint32_t)];
    } test_endian{1};

    return test_endian.c[0] == 1;
}

class endian {
public:
    explicit endian(bool little_endian = host_is_little_endian())
        : m_little_endian{little_endian}
    {
        m_to_uint32 = little_endian ? to_uint32_le : to_uint32_be;
        m_to_uint64 = little_endian ? to_uint64_le : to_uint64_be;
    }

    uint32_t to_uint32(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) const
    {
        return m_to_uint32(byte1, byte2, byte3, byte4);
    }

    uint64_t to_uint64(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4,
                        uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8) const
    {
        return m_to_uint64(byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8);
    }

private:
    using cast_uint32_t = uint32_t(*)(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4);
    using cast_uint64_t = uint64_t(*)(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
                                        uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8);

    cast_uint32_t m_to_uint32;
    cast_uint64_t m_to_uint64;
    bool m_little_endian;

    static uint32_t to_uint32_le(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4)
    {
        uint32_t r = v4;
        r = (r << 8) | v3;
        r = (r << 8) | v2;
        r = (r << 8) | v1;
        return r;
    }

    static uint32_t to_uint32_be(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4)
    {
        return to_uint32_le(v4, v3, v2, v1);
    }

    static uint64_t to_uint64_le(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
                                    uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8)
    {
        uint64_t r = v8;
        r = (r << 8) | v8;
        r = (r << 8) | v7;
        r = (r << 8) | v6;
        r = (r << 8) | v5;
        r = (r << 8) | v4;
        r = (r << 8) | v3;
        r = (r << 8) | v2;
        r = (r << 8) | v1;
        return r;
    }

    static uint64_t to_uint64_be(uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4,
                                    uint8_t v5, uint8_t v6, uint8_t v7, uint8_t v8)
    {
        return to_uint64_le(v8, v7, v6, v5, v4, v3, v2, v1);
    }

};

} // namespace __detail

// exports
using endian_t = __detail::endian;
static const endian_t endian_le{true};
static const endian_t endian_be{false};

CPPLUA_NS_END

#endif

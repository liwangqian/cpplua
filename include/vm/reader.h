#ifndef CPPLUA_VM_READER_H
#define CPPLUA_VM_READER_H

#include "def.h"
#include "endian.h"
#include "config.h"
#include <vector>
#include <fstream>

CPPLUA_NS_BEGIN
namespace __detail {

class binary_reader {
public:
    binary_reader();
    ~binary_reader();

    bool input(const char *file)
    {
        std::ifstream in{file, std::ios::in|std::ios::binary|std::ios::ate};
        if (!in) {
            return false;
        }

        auto length = in.tellg();
        in.seekg(0, std::ios::beg);
        m_bytes.resize(length);
        in.read(m_bytes.data(), length);
        in.close();

        m_offset = 0;
        return true;
    }

    uint8_t read_byte()
    {
        return m_bytes[m_offset++];
    }

    uint32_t read_uint32()
    {
        auto v1 = read_byte();
        auto v2 = read_byte();
        auto v3 = read_byte();
        auto v4 = read_byte();
        return m_endian.to_uint32(v1, v2, v3, v4);
    }

    uint64_t read_uint64()
    {
        auto v1 = read_byte();
        auto v2 = read_byte();
        auto v3 = read_byte();
        auto v4 = read_byte();
        auto v5 = read_byte();
        auto v6 = read_byte();
        auto v7 = read_byte();
        auto v8 = read_byte();
        return m_endian.to_uint64(v1, v2, v3, v4, v5, v6, v7, v8);
    }

    lua_integer_t read_lua_integer()
    {
        return read_uint64();
    }

    lua_number_t read_lua_number()
    {
        return *(lua_number_t*)(read_uint64());
    }

private:
    using byte_table_t = std::vector<char>;
    byte_table_t m_bytes;
    uint32_t m_offset = 0;
    const endian_t m_endian;
};

} // namespace __detail

// exports
using reader_t = __detail::binary_reader;

CPPLUA_NS_END

#endif

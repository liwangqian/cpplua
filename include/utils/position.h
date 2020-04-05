//
// Created by busted on 2020/4/5.
//

#ifndef CPPLUA_POSITION_H
#define CPPLUA_POSITION_H

#include "def.h"
#include "json.hpp"
#include "fmt/format.h"

CPPLUA_NS_BEGIN

namespace __detail {

struct position {
    std::uint32_t line{0};   /* 行号 */
    std::uint32_t column{0}; /* 列号 */
    std::uint32_t offset{0}; /* 与第一个字符的偏移量 */

    inline bool operator==(const position &other) const
    {
        return line == other.line && column == other.column && offset == other.offset;
    }

    inline bool operator!=(const position &other) const
    {
        return !this->operator==(other);
    }

    inline bool operator<(const position &other) const
    {
        return offset < other.offset;
    }

    inline bool operator>(const position &other) const
    {
        return offset > other.offset;
    }

    inline bool operator<=(const position &other) const
    {
        return offset <= other.offset;
    }

    inline bool operator>=(const position &other) const
    {
        return offset >= other.offset;
    }

    inline string_t to_string() const
    {
        return fmt::format("{} line = {}, column = {}, offset = {} {}", '{', line, column, offset, '}');
    }

};

inline void to_json(nlohmann::json &json, const position &p)
{
    json["line"] = p.line;
    json["column"] = p.column;
    json["offset"] = p.offset;
}

} // namespace __detail

// exports
using position_t = __detail::position;

CPPLUA_NS_END

namespace fmt {
template<>
struct formatter<cpplua::position_t>
    : formatter<string_view> {
    template<typename FormatContext>
    auto format(const cpplua::position_t &p, FormatContext &ctx)
    {
        return formatter<string_view>::format(p.to_string(), ctx);
    }
};
}

#endif //CPPLUA_POSITION_H

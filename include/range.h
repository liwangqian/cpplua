#ifndef CPPLUA_RANGE_H
#define CPPLUA_RANGE_H

#include "def.h"
#include <array>
#include "fmt/format.h"
#include "json.hpp"

CPPLUA_NS_BEGIN

namespace __detail {

struct position {
    std::uint32_t line{0};   /* 行号 */
    std::uint32_t column{0}; /* 列号 */
    std::uint32_t offset{0}; /* 与第一个字符的偏移量 */

    inline bool operator==(const position &other)
    {
        return line == other.line && column == other.column && offset == other.offset;
    }

    inline bool operator!=(const position &other)
    {
        return !this->operator==(other);
    }

    inline bool operator<(const position &other)
    {
        return offset < other.offset;
    }

    inline bool operator>(const position &other)
    {
        return offset > other.offset;
    }

    inline bool operator<=(const position &other)
    {
        return offset <= other.offset;
    }

    inline bool operator>=(const position &other)
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

struct range {
    position start;
    position end;

    inline bool operator==(const range &r)
    {
        return start == r.start && end == r.end;
    }

    inline bool operator!=(const range &r)
    {
        return start != r.start || end != r.end;
    }

    inline bool contains(const range &r)
    {
        return start <= r.start && end >= r.end;
    }

    inline string_t to_string() const
    {
        return fmt::format("{} start = {}, end = {} {}", '{', start.to_string(), end.to_string(), '}');
    }
};

inline void to_json(nlohmann::json &json, const range &r)
{
    json["start"] = r.start;
    json["end"] = r.end;
}

} // namespace __detail

// exports
using position_t = __detail::position;
using range_t = __detail::range;

CPPLUA_NS_END

#endif

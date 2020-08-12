#ifndef CPPLUA_RANGE_H
#define CPPLUA_RANGE_H

#include "def.h"
#include "position.h"
#include "fmt/format.h"
#include "json.hpp"

CPPLUA_NS_BEGIN

namespace __detail {

template <typename T>
struct range {
    T start{};
    T end{};

    inline bool operator==(const range &r) const
    {
        return start == r.start && end == r.end;
    }

    inline bool operator!=(const range &r) const
    {
        return start != r.start || end != r.end;
    }

    inline bool contains(const range &r) const
    {
        return start <= r.start && end >= r.end;
    }

    inline string_t to_string() const
    {
        return fmt::format("{} start = {}, end = {} {}", '{', start, end, '}');
    }
};

template <typename T>
inline void to_json(nlohmann::json &json, const range<T> &r)
{
    json["start"] = r.start;
    json["end"] = r.end;
}

} // namespace __detail

// exports
using range_t = __detail::range<position_t>;
using vrange_t = __detail::range<uint32_t>;

CPPLUA_NS_END

namespace fmt {
template<typename T>
struct formatter<cpplua::__detail::range<T>>
    : formatter<string_view> {
    template<typename FormatContext>
    auto format(const cpplua::__detail::range<T> &r, FormatContext &ctx)
    {
        return formatter<string_view>::format(r.to_string(), ctx);
    }
};
}

#endif

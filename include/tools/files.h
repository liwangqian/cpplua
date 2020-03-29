//
// Created by busted on 2020/2/23.
//

#ifndef CPPLUA_FILES_H
#define CPPLUA_FILES_H

#include "def.h"
#include "resolver.h"
#include <algorithm>

CPPLUA_NS_BEGIN
namespace tools {

struct file_t {
    const cpplua::string_t path;
    uint32_t refs{0};

    std::size_t distance(const file_t &o) const
    {
        auto min_size = std::min(path.size(), o.path.size());
        std::size_t i = 0;
        for (; i < min_size; ++i) {
            if (path[i] != o.path[i]) {
                break;
            }
        }
        auto c1 = std::count(path.begin() + i, path.end(), '/');
        auto c2 = std::count(o.path.begin() + i, o.path.end(), '/');
        return c1 + c2;
    }
};

using file_resolver_t = resolver_t<cpplua::string_t, file_t>;

inline auto resolve_file(const cpplua::string_t &path)
{
    return file_resolver_t::instance().resolve(path);
}

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_FILES_H

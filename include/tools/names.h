//
// Created by busted on 2020/2/23.
//

#ifndef CPPLUA_NAMES_H
#define CPPLUA_NAMES_H

#include "def.h"
#include "singleton.h"
#include "resolver.h"

CPPLUA_NS_BEGIN
namespace tools {

struct name_t {
    const cpplua::string_t value;
    uint32_t refs{0};
};

using name_resolver_t = resolver_t<cpplua::string_t, name_t>;
inline auto resolve_name(const cpplua::string_t &name)
{
    return name_resolver_t::instance().resolve(name);
}

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_NAMES_H

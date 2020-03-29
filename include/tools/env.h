//
// Created by busted on 2020/3/7.
//

#ifndef CPPLUA_ENV_H
#define CPPLUA_ENV_H

#include "def.h"
#include <unordered_map>

CPPLUA_NS_BEGIN
namespace tools {

struct symbol_t;

struct env_t {
    explicit env_t(env_t *p) : prev{p}
    {
        stack_depth = p ? (p->stack_depth + 1) : 0;
    }

    void put(const cpplua::string_t &name, symbol_t* symbol)
    {
        scope[name] = symbol;
    }

    symbol_t *get(const cpplua::string_t &name)
    {
        for (auto e = this; e != nullptr; e = e->prev) {
            auto iter = e->scope.find(name);
            if (iter != e->scope.end()) {
                return iter->second;
            }
        }
        return nullptr;
    }

    env_t *prev_env()
    {
        return prev;
    }

    auto depth() const
    {
        return stack_depth;
    }

    template <typename F>
    void foreach(F &&f)
    {
        for (auto &i : scope) {
            f(i.second);
        }
    }

private:
    std::size_t stack_depth{0};
    env_t *prev{nullptr};
    std::unordered_map<cpplua::string_t, symbol_t*> scope;
};

} // namespace tools
CPPLUA_NS_END

#endif //CPPLUA_ENV_H

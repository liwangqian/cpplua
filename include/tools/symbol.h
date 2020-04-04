//
// Created by busted on 2020/2/18.
//

#ifndef CPPLUA_SYMBOL_H
#define CPPLUA_SYMBOL_H

#include "def.h"
#include "range.h"
#include "factory.h"
#include "names.h"
#include "files.h"
#include "env.h"

CPPLUA_NS_BEGIN
namespace tools {

struct location_t {
    const file_t *file{nullptr};
    range_t range;
};

enum class symbol_type_t {
    any,
    number,
    string,
    boolean,
    table,
    function,
    multi,
    group,
    returns_of,
    ref,
    undefined,
};

struct symbol_t {
    symbol_t() = default;
    symbol_t(const name_t *name, env_t *env)
        : name{name}, env{env}
    {
    }

    template <typename DerivedPointer>
    DerivedPointer cast_to_ptr()
    {
        return static_cast<DerivedPointer>(this);
    }

    virtual cpplua::string_t to_string() const = 0;
    virtual cpplua::string_t type_string() const = 0;

    const name_t *name{nullptr};
    env_t *env{nullptr};
    location_t location;
    std::forward_list<location_t> references;
    symbol_type_t type{symbol_type_t::any};
    bool is_local{false};
};

struct table_t : public symbol_t {
    table_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::table;
    }

    symbol_t *find_field(const name_t *name)
    {
        for (const auto s : fields) {
            if (s->name == name) {
                return s;
            }
        }
        return nullptr;
    }

    cpplua::string_t to_string() const override
    {
        if (name == nullptr) {
            return type_string();
        }
        return fmt::format("{}: {}", name->value, type_string());
    }

    cpplua::string_t type_string() const override
    {
        if (fields.empty()) {
            return "{...}";
        }
        if (fields.size() > 5) {
            return fmt::format("{}\n{}\n...\n{}", '{',
                fmt::join(fields.begin(), fields.begin() + 5, ",\n"), '}');
        }
        return fmt::format("{}\n{}\n{}", '{', fmt::join(fields, ",\n"), '}');
    }

    std::vector<symbol_t*> fields;
};

struct func_t : public symbol_t {
    func_t(const name_t *name, env_t *env)
        : symbol_t{name, env}, fenv{env}
    {
        type = symbol_type_t::function;
    }

    std::vector<symbol_t*> params;
    std::vector<symbol_t*> returns;
    env_t fenv;

    cpplua::string_t to_string() const override
    {
        if (name) {
            return fmt::format("{}: {}", name->value, format());
        }
        return type_string();
    }

    cpplua::string_t type_string() const override
    {
        return format();
    }

private:
    cpplua::string_t format() const
    {
        fmt::memory_buffer mb;
        fmt::format_to(mb, "function({}) -> ", fmt::join(params, ", "));
        if (!returns.empty()) {
            fmt::format_to(mb, "{}", fmt::join(returns, ", "));
        } else {
            fmt::format_to(mb, "void");
        }
        return fmt::to_string(mb);
    }
};

struct str_t : public symbol_t {
    str_t() : str_t{nullptr, nullptr} {}

    str_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::string;
    }

    cpplua::string_t to_string() const override
    {
        return (name != nullptr) ? fmt::format("{}: string", name->value) : "string";
    }

    cpplua::string_t type_string() const override
    {
        return "string";
    }
};

struct any_t : public symbol_t {
    any_t() : any_t{nullptr, nullptr} {}
    any_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::any;
    }

    cpplua::string_t to_string() const override
    {
        return (name != nullptr) ? fmt::format("{}: any", name->value) : "any";
    }

    cpplua::string_t type_string() const override
    {
        return "any";
    }
};

struct number_t : public symbol_t {
    number_t() : number_t{nullptr, nullptr} {}
    number_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::number;
    }

    cpplua::string_t to_string() const override
    {
        return (name != nullptr) ? fmt::format("{}: number", name->value) : "number";
    }

    cpplua::string_t type_string() const override
    {
        return "number";
    }
};

struct boolean_t : public symbol_t {
    boolean_t() : boolean_t{nullptr, nullptr} {}
    boolean_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::boolean;
    }

    cpplua::string_t to_string() const override
    {
        return (name != nullptr) ? fmt::format("{}: boolean", name->value) : "boolean";
    }

    cpplua::string_t type_string() const override
    {
        return "boolean";
    }
};

struct multi_type_t : public symbol_t {
    template <typename ...S>
    explicit multi_type_t(S ...s) : symbols{{s...}}
    {
        type = symbol_type_t::multi;
    }

    cpplua::string_t to_string() const override
    {
        return fmt::format("{}", fmt::join(symbols, "|"));
    }

    cpplua::string_t type_string() const override
    {
        return to_string();
    }

    std::vector<symbol_t *> symbols;
};

struct symbol_group_t : public symbol_t {
    template <typename ...S>
    explicit symbol_group_t(S ...s) : symbols{{s...}}
    {
        type = symbol_type_t::group;
    }

    explicit symbol_group_t(std::vector<symbol_t *> &&s)
        : symbols{s}
    {
        type = symbol_type_t::group;
    }

    cpplua::string_t to_string() const override
    {
        return type_string();
    }

    cpplua::string_t type_string() const override
    {
        return "{...}";
    }

    std::vector<symbol_t *> symbols;
};

struct returns_of_t : public symbol_t {
    returns_of_t(uint8_t index, const name_t *name, env_t *env)
        : symbol_t{name, env}, index{index}
    {
        type = symbol_type_t::returns_of;
    }

    cpplua::string_t to_string() const override
    {
        return (resolved == nullptr) ? fmt::format("{}: any", name->value) : resolved->to_string();
    }

    cpplua::string_t type_string() const override
    {
        return (resolved == nullptr) ? "any" : resolved->type_string();
    }

    bool try_resolve()
    {
        auto s = env->get(name->value);
        if (s != nullptr && s->type == symbol_type_t::function) {
            auto fp = dynamic_cast<func_t*>(s);
            if (fp->returns.size() > index) {
                resolved = fp->returns[index];
                return resolved != nullptr;
            }
        }
        return false;
    }

    const uint8_t index;
    const symbol_t *resolved{nullptr};
};

struct ref_t : public symbol_t {
    ref_t(const name_t *name, env_t *env, symbol_t *ref)
        : symbol_t{name, env}, ref{ref}
    {
        type = symbol_type_t::ref;
    }

    cpplua::string_t to_string() const override
    {
        return fmt::format("{}: {}", name->value, ref->type_string());
    }

    cpplua::string_t type_string() const override
    {
        return ref->type_string();
    }

    symbol_type_t ref_type() const
    {
        if (ref == nullptr) return symbol_type_t::any;
        if (ref->type == symbol_type_t::ref) {
            return ref->cast_to_ptr<ref_t*>()->ref_type();
        }
        return ref->type;
    }

    symbol_t *def()
    {
        if (ref == nullptr) return nullptr;
        if (ref->type == symbol_type_t::ref) {
            return ref->cast_to_ptr<ref_t*>()->def();
        }
        return ref;
    }

    symbol_t *ref{nullptr};
};

struct undefined_t : public symbol_t {
    undefined_t(const name_t *name, env_t *env)
        : symbol_t{name, env}
    {
        type = symbol_type_t::undefined;
    }

    cpplua::string_t to_string() const override
    {
        return fmt::format("{}: {}", name->value, res ? res->type_string() : "any");
    }

    cpplua::string_t type_string() const override
    {
        return res ? res->type_string() : "any";
    }

    bool try_resolve()
    {
        res = env->get(name->value);
        return res != nullptr;
    }

    symbol_t *res{nullptr};
};

using symbol_factory_t = factory_t<symbol_t>;
template <typename S, typename ...Args>
decltype(auto) create_symbol(Args&& ...args)
{
    return symbol_factory_t::instance().create<S>(args...);
}

inline bool is_table(symbol_t *s)
{
    return s->type == symbol_type_t::table;
}

inline bool is_function(symbol_t *s)
{
    return s->type == symbol_type_t::function;
}

inline bool is_ref(symbol_t *s)
{
    return s->type == symbol_type_t::ref;
}

inline bool is_multi(symbol_t *s)
{
    return s->type == symbol_type_t::multi;
}

inline bool is_group(symbol_t *s)
{
    return s->type == symbol_type_t::group;
}

} // namespace tools

CPPLUA_NS_END

namespace fmt {
template<>
struct formatter<cpplua::tools::symbol_t*>
    : formatter<string_view> {
    template<typename FormatContext>
    auto format(const cpplua::tools::symbol_t *d, FormatContext &ctx)
    {
        return formatter<string_view>::format(d->to_string(), ctx);
    }
};

template<>
struct formatter<cpplua::tools::symbol_t>
    : formatter<string_view> {
    template<typename FormatContext>
    auto format(const cpplua::tools::symbol_t &d, FormatContext &ctx)
    {
        return formatter<string_view>::format(d.to_string(), ctx);
    }
};

}

#endif //CPPLUA_SYMBOL_H

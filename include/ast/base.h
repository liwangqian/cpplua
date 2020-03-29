#ifndef CPPLUA_BASE_H
#define CPPLUA_BASE_H

#include "def.h"
#include "range.h"
#include <memory>
#include <cassert>
#include <sstream>
#include "json.hpp"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

enum node_type {
    stmt_label,
    stmt_break,
    stmt_goto,
    stmt_return,
    stmt_if,
    stmt_if_clause,
    stmt_elseif_clause,
    stmt_else_clause,
    stmt_do,
    stmt_while,
    stmt_repeat,
    stmt_local,
    stmt_assignment,
    stmt_call,
    stmt_function,
    stmt_for_numeric,
    stmt_for_generic,
    stmt_chunk,
    stmt_ident,
    stmt_literal,
    stmt_table_key,
    stmt_table_key_string,
    stmt_table_value,

    expr_table_constructor,
    expr_binary,
    expr_unary,
    expr_member,
    expr_index,
    expr_call,
    expr_table_call,
    expr_string_call,

    comment,

    node_type_max
};

struct base_node {
    node_type type;
    range_t range;
    bool is_local;

    base_node(node_type type)
        : type{type}
    {
    }

    bool is_type(node_type target)
    {
        return target == type;
    }

    template<typename child>
    const child &cast_to() const
    {
        assert(child::class_type == type);
        return static_cast<const child &>(*this);
    }

    template<typename child>
    child &cast_to()
    {
        assert(child::class_type == type);
        return static_cast<child &>(*this);
    }

    virtual void to_json(nlohmann::json &json) const
    {
        json["type"] = type;
        json["range"] = range;
    }
};

} // namespace __detail

// exports
using node_type_t = __detail::node_type;
using node_ptr_t = std::shared_ptr<__detail::base_node>;

namespace __detail {

inline void to_json(nlohmann::json &json, const node_ptr_t &v)
{
    if (v) v->to_json(json);
}

}

} // namespace ast

CPPLUA_NS_END
#endif

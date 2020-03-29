#ifndef CPPLUA_CALL_H
#define CPPLUA_CALL_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class callstmt_node : public base_node {
public:
    static constexpr node_type class_type = stmt_call;

    const node_ptr_t &expression() const
    {
        return m_expr;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["expression"] = m_expr;
    }

    explicit callstmt_node(node_ptr_t expr)
        : base_node{class_type}, m_expr{std::move(expr)}
    {
    }

private:
    node_ptr_t m_expr;
};

class callexpr_node : public base_node {
public:
    static constexpr node_type class_type = expr_call;

    const std::vector<node_ptr_t> &args() const
    {
        return m_args;
    }

    const node_ptr_t &base() const
    {
        return m_base;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["base"] = m_base;
        json["arguments"] = m_args;
    }

    callexpr_node(node_ptr_t base, std::vector<node_ptr_t> args)
        : base_node{class_type}, m_base{std::move(base)}, m_args{std::move(args)}
    {
    }

private:
    node_ptr_t m_base;
    std::vector<node_ptr_t> m_args;
};

class singleparam_call_base : public base_node {
public:
    const node_ptr_t &arg() const
    {
        return m_arg;
    }

    const node_ptr_t &base() const
    {
        return m_base;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["base"] = m_base;
        json["argument"] = m_arg;
    }

    singleparam_call_base(node_type class_type, node_ptr_t base, node_ptr_t arg)
        : base_node{class_type}, m_base{std::move(base)}, m_arg{std::move(arg)}
    {
    }

private:
    node_ptr_t m_base;
    node_ptr_t m_arg;
};

class stringcall_node : public singleparam_call_base {
public:
    static constexpr node_type class_type = expr_string_call;
    using supper = singleparam_call_base;

    stringcall_node(node_ptr_t base, node_ptr_t arg)
        : singleparam_call_base{class_type, std::move(base), std::move(arg)}
    {
    }
};

class tablecall_node : public singleparam_call_base {
public:
    static constexpr node_type class_type = expr_table_call;
    using supper = singleparam_call_base;

    tablecall_node(node_ptr_t base, node_ptr_t arg)
        : singleparam_call_base{class_type, std::move(base), std::move(arg)}
    {
    }
};

} // namespace __detail

// exports
using callstmt_t = __detail::callstmt_node;
using callexpr_t = __detail::callexpr_node;
using string_call_t = __detail::stringcall_node;
using table_call_t = __detail::tablecall_node;

} // namespace ast

CPPLUA_NS_END
#endif

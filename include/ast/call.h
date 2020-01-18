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

    callstmt_node(node_ptr_t expr)
        : base_node{class_type}, m_expr{std::move(expr)}
    {
    }

    ~callstmt_node() = default;

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

    ~callexpr_node() = default;

private:
    node_ptr_t m_base;
    std::vector<node_ptr_t> m_args;
};

class stringcall_node : public base_node {
public:
    static constexpr node_type class_type = expr_string_call;

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

    stringcall_node(node_ptr_t base, node_ptr_t arg)
        : base_node{class_type}, m_base{std::move(base)}, m_arg{std::move(arg)}
    {
    }

    ~stringcall_node() = default;

private:
    node_ptr_t m_base;
    node_ptr_t m_arg;
};

class tablecall_node : public base_node {
public:
    static constexpr node_type class_type = expr_string_call;

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

    tablecall_node(node_ptr_t base, node_ptr_t arg)
        : base_node{class_type}, m_base{std::move(base)}, m_arg{std::move(arg)}
    {
    }

    ~tablecall_node() = default;

private:
    node_ptr_t m_base;
    node_ptr_t m_arg;
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

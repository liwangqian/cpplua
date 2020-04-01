#ifndef CPPLUA_UNARY_H
#define CPPLUA_UNARY_H

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class unary_node : public base_node {
public:
    static constexpr node_type class_type = expr_unary;

    const string_t &op() const
    {
        return m_op;
    }

    const node_ptr_t &arg() const
    {
        return m_arg;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["operator"] = m_op;
        json["argument"] = m_arg;
    }

    unary_node(const string_t &name, node_ptr_t arg)
        : base_node{class_type}, m_op{name}, m_arg{std::move(arg)}
    {
    }

    ~unary_node() = default;

private:
    string_t m_op;
    node_ptr_t m_arg;
};

} // namespace __detail

// exports
using unary_t = __detail::unary_node;

} // namespace ast

CPPLUA_NS_END
#endif

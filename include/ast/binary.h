#ifndef CPPLUA_BINARY_H
#define CPPLUA_BINARY_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class binaryop_node : public base_node {
public:
    static constexpr node_type class_type = expr_binary;

    const node_ptr_t &left() const
    {
        return m_left;
    }

    const node_ptr_t &right() const
    {
        return m_right;
    }

    const string_t &oper() const
    {
        return m_oper;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["operator"] = m_oper;
        json["left"] = m_left;
        json["right"] = m_right;
    }

    explicit binaryop_node(string_t oper, node_ptr_t left, node_ptr_t right)
        : base_node{class_type}, m_oper{std::move(oper)}, m_left{std::move(left)}, m_right{std::move(right)}
    {
    }

    ~binaryop_node() = default;
private:
    string_t m_oper;
    node_ptr_t m_left;
    node_ptr_t m_right;
};

} // namespace __detail

// exports
using binaryop_t = __detail::binaryop_node;

} // namespace ast

CPPLUA_NS_END
#endif

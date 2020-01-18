#ifndef CPPLUA_WHILE_H
#define CPPLUA_WHILE_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class whilestmt_node : public base_node {
public:
    const node_ptr_t &condition() const
    {
        return m_condition;
    }

    const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["condition"] = m_condition;
        json["body"] = m_body;
    }

    whilestmt_node(node_ptr_t condition, std::vector<node_ptr_t> body)
        : base_node{class_type}, m_condition{std::move(condition)}, m_body{std::move(body)}
    {
    }

    ~whilestmt_node() = default;

    static constexpr node_type class_type = stmt_while;

private:
    node_ptr_t m_condition;
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using whilestmt_t = __detail::whilestmt_node;

} // namespace ast

CPPLUA_NS_END
#endif

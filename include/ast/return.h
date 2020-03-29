#ifndef CPPLUA_RETURN_H
#define CPPLUA_RETURN_H

#include "base.h"
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class return_node : public base_node {
public:
    static constexpr node_type class_type = stmt_return;

    const std::vector<node_ptr_t> &expressions() const
    {
        return m_expressions;
    }

    void push(const node_ptr_t &expr)
    {
        m_expressions.push_back(expr);
        return;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["expressions"] = m_expressions;
    }

    return_node()
        : base_node{class_type}
    {
    }

    ~return_node() = default;

private:
    std::vector<node_ptr_t> m_expressions;
};

} // namespace __detail

// exports
using returnstmt_t = __detail::return_node;

} // namespace ast

CPPLUA_NS_END
#endif

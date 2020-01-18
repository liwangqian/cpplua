#ifndef CPPLUA_ASSIGN_H
#define CPPLUA_ASSIGN_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class assignment_node : public base_node {
public:
    static constexpr node_type class_type = stmt_assignment;

    const std::vector<node_ptr_t> &vars() const
    {
        return m_vars;
    }

    const std::vector<node_ptr_t> &inits() const
    {
        return m_inits;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["variables"] = m_vars;
        json["init"] = m_inits;
    }

    assignment_node(std::vector<node_ptr_t> vars, std::vector<node_ptr_t> inits)
        : base_node{class_type}, m_vars{std::move(vars)}, m_inits{std::move(inits)}
    {
        this->is_local = is_local;
    }

    ~assignment_node() = default;

private:
    std::vector<node_ptr_t> m_vars;
    std::vector<node_ptr_t> m_inits;
};

} // namespace __detail

// exports
using assignment_t = __detail::assignment_node;

} // namespace ast

CPPLUA_NS_END
#endif

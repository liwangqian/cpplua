#ifndef CPPLUA_LOCAL_H
#define CPPLUA_LOCAL_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class local_node : public base_node {
public:
    static constexpr node_type class_type = stmt_local;

    const std::vector<node_ptr_t> &variables() const
    { return m_variables; }

    const std::vector<node_ptr_t> &init() const
    { return m_init; }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["variables"] = m_variables;
        json["init"] = m_init;
    }

    local_node(std::vector<node_ptr_t> vars, std::vector<node_ptr_t> inits)
        : base_node{class_type}, m_variables{std::move(vars)}, m_init{std::move(inits)}
    {
        is_local = true;
    }

private:
    std::vector<node_ptr_t> m_variables;
    std::vector<node_ptr_t> m_init;
};

} // namespace __detail

// exports
using local_t = __detail::local_node;

} // namespace ast

CPPLUA_NS_END
#endif

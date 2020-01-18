#ifndef CPPLUA_REPEAT_H
#define CPPLUA_REPEAT_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class repeat_node : public base_node {
public:
    static constexpr node_type class_type = stmt_repeat;

    inline const node_ptr_t &condition() const
    {
        return m_condition;
    }

    inline const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["condition"] = m_condition;
        json["body"] = m_body;
    }

    repeat_node(node_ptr_t condition, std::vector<node_ptr_t> body)
        : base_node{class_type}, m_condition{std::move(condition)}, m_body{std::move(body)}
    {
    }

    ~repeat_node() = default;

private:
    node_ptr_t m_condition;
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using repeat_t = __detail::repeat_node;

} // namespace ast

CPPLUA_NS_END
#endif

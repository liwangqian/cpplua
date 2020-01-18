#ifndef CPPLUA_DOSTMT_H
#define CPPLUA_DOSTMT_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class dostmt_node : public base_node {
public:
    static constexpr node_type class_type = stmt_do;

    const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["body"] = m_body;
    }

    dostmt_node(std::vector<node_ptr_t> body)
        : base_node{class_type}, m_body{std::move(body)}
    {
    }

    ~dostmt_node() = default;

private:
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using dostmt_t = __detail::dostmt_node;

} // namespace ast

CPPLUA_NS_END
#endif

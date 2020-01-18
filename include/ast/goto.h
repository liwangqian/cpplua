#ifndef CPPLUA_GOTO_H
#define CPPLUA_GOTO_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class gotostmt_node : public base_node {
public:
    static constexpr node_type class_type = stmt_goto;

    const node_ptr_t &label() const
    {
        return m_label;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["label"] = m_label;
    }

    explicit gotostmt_node(node_ptr_t label)
        : base_node{class_type}, m_label{std::move(label)}
    {
    }

    ~gotostmt_node() = default;

private:
    node_ptr_t m_label;
};

} // namespace __detail

// exports
using gotostmt_t = __detail::gotostmt_node;

} // namespace ast

CPPLUA_NS_END
#endif

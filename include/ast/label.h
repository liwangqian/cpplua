#ifndef CPPLUA_LABEL_H
#define CPPLUA_LABEL_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class label_node : public base_node {
public:
    static constexpr node_type class_type = stmt_label;

    const node_ptr_t &label() const
    {
        return m_label;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["label"] = m_label;
    }

    explicit label_node(node_ptr_t label)
        : base_node{class_type}, m_label{std::move(label)}
    {
    }

    ~label_node() = default;

private:
    node_ptr_t m_label;
};

} // namespace __detail

// exports
using label_t = __detail::label_node;

} // namespace ast

CPPLUA_NS_END
#endif

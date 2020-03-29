#ifndef CPPLUA_IDENT_H
#define CPPLUA_IDENT_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class ident_node : public base_node {
public:
    static constexpr node_type class_type = stmt_ident;

    const string_t &name() const
    {
        return m_name;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["name"] = m_name;
    }

    explicit ident_node(string_t name)
        : base_node{class_type}, m_name{std::move(name)}
    {
    }

    ~ident_node() = default;
private:
    string_t m_name;
};

} // namespace __detail

// exports
using ident_t = __detail::ident_node;

} // namespace ast

CPPLUA_NS_END
#endif

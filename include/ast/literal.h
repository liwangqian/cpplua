#ifndef CPPLUA_LITERAL_H
#define CPPLUA_LITERAL_H

#include "base.h"
#include "token.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class literal_node : public base_node {
public:
    static constexpr node_type class_type = stmt_literal;

    inline token_type_t value_type() const
    {
        return m_value_type;
    }

    inline const string_t &value() const
    {
        return m_value;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["value_type"] = m_value_type;
        json["value"] = m_value;
    }

    literal_node(token_type_t type, const string_t &value)
        : base_node{class_type}, m_value_type{type}, m_value{value}
    {
    }

    ~literal_node() = default;

private:
    token_type_t m_value_type;
    string_t m_value;
};

} // namespace __detail

// exports
using literal_t = __detail::literal_node;

} // namespace ast

CPPLUA_NS_END
#endif

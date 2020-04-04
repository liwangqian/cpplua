#ifndef CPPLUA_MEMBER_H
#define CPPLUA_MEMBER_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class member_node : public base_node {
public:
    static constexpr node_type class_type = expr_member;

    const node_ptr_t &base() const
    {
        return m_base;
    }

    const node_ptr_t &identifier() const
    {
        return m_ident;
    }

    const string_t &indexer() const
    {
        return m_indexer;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["base"] = m_base;
        json["identifier"] = m_ident;
        json["indexer"] = m_indexer;
    }

    member_node(node_ptr_t base, string_t indexer, node_ptr_t ident)
        : base_node{class_type}, m_base{std::move(base)}
        , m_ident{std::move(ident)}, m_indexer{std::move(indexer)}
    {
    }

private:
    node_ptr_t m_base;
    node_ptr_t m_ident;
    string_t m_indexer;
};

} // namespace __detail

// exports
using member_t = __detail::member_node;

} // namespace ast

CPPLUA_NS_END
#endif

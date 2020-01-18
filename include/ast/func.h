#ifndef CPPLUA_FUNC_H
#define CPPLUA_FUNC_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class funcdecl_node : public base_node {
public:
    static constexpr node_type class_type = stmt_function;

    const node_ptr_t &identifier() const
    {
        return m_ident;
    }

    const std::vector<node_ptr_t> &params() const
    {
        return m_params;
    }

    const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["identifier"] = m_ident;
        json["parameters"] = m_params;
        json["body"] = m_body;
    }

    funcdecl_node(node_ptr_t ident, std::vector<node_ptr_t> params, std::vector<node_ptr_t> body, bool is_local)
        : base_node{class_type}, m_ident{std::move(ident)}, m_params{std::move(params)}, m_body{std::move(body)}
    {
        this->is_local = is_local;
    }

    ~funcdecl_node() = default;

private:
    node_ptr_t m_ident;
    std::vector<node_ptr_t> m_params;
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using funcdecl_t = __detail::funcdecl_node;

} // namespace ast

CPPLUA_NS_END
#endif

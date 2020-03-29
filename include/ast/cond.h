#ifndef CPPLUA_AST_IFCLAUSE_H
#define CPPLUA_AST_IFCLAUSE_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class ifstmt_node : public base_node {
public:
    static constexpr node_type class_type = stmt_if;

    void add_clause(const node_ptr_t &node)
    { m_clauses.push_back(node); }

    const std::vector<node_ptr_t>& clauses() const
    {
        return m_clauses;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["clauses"] = m_clauses;
    }

    ifstmt_node()
        : base_node{class_type}
    {
    }

    ~ifstmt_node() = default;

private:
    std::vector<node_ptr_t> m_clauses;
};

class ifclause_base_node : public base_node {
public:
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

    ifclause_base_node(node_type class_type, node_ptr_t condition, std::vector<node_ptr_t> body)
        : base_node{class_type}, m_condition{std::move(condition)}, m_body{std::move(body)}
    {
    }

    ~ifclause_base_node() = default;

private:
    node_ptr_t m_condition;
    std::vector<node_ptr_t> m_body;
};

class ifclause_node : public ifclause_base_node {
public:
    static constexpr node_type class_type = stmt_if_clause;

    ifclause_node(const node_ptr_t &condition, std::vector<node_ptr_t> body)
        : ifclause_base_node{class_type, condition, std::move(body)}
    {
    }

    ~ifclause_node() = default;
};

class elseifclause_node : public ifclause_base_node {
public:
    static constexpr node_type class_type = stmt_elseif_clause;

    elseifclause_node(node_ptr_t condition, std::vector<node_ptr_t> &body)
        : ifclause_base_node{class_type, std::move(condition), std::move(body)}
    {
    }

    ~elseifclause_node() = default;
};

class elseclause_node : public base_node {
public:
    static constexpr node_type class_type = stmt_else_clause;

    inline const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["body"] = m_body;
    }

    elseclause_node(std::vector<node_ptr_t> body)
        : base_node{class_type}, m_body{std::move(body)}
    {
    }

    ~elseclause_node() = default;

private:
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using ifstmt_t = __detail::ifstmt_node;
using ifclause_t = __detail::ifclause_node;
using elseifclause_t = __detail::elseifclause_node;
using elseclause_t = __detail::elseclause_node;

} // namespace ast

CPPLUA_NS_END
#endif

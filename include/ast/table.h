#ifndef CPPLUA_TABLE_H
#define CPPLUA_TABLE_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class table_construct_expr_node : public base_node {
public:
    static constexpr node_type class_type = expr_table_constructor;

    const std::vector<node_ptr_t> &fields() const
    {
        return m_fields;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["fields"] = m_fields;
    }

    table_construct_expr_node(std::vector<node_ptr_t> fields)
        : base_node{class_type}, m_fields{std::move(fields)}
    {
    }

    ~table_construct_expr_node() = default;

private:
    std::vector<node_ptr_t> m_fields;
};

class tablekey_node : public base_node {
public:
    static constexpr node_type class_type = stmt_table_key;

    const node_ptr_t &key() const
    {
        return m_key;
    }

    const node_ptr_t &value() const
    {
        return m_value;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["key"] = m_key;
        json["value"] = m_value;
    }

    tablekey_node(node_ptr_t key, node_ptr_t value)
        : base_node{class_type}, m_key{std::move(key)}, m_value{std::move(value)}
    {
    }

    ~tablekey_node() = default;

private:
    node_ptr_t m_key;
    node_ptr_t m_value;
};

class tablekey_string_node : public base_node {
public:
    static constexpr node_type class_type = stmt_table_key_string;

    const node_ptr_t &key() const
    {
        return m_key;
    }

    const node_ptr_t &value() const
    {
        return m_value;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["key"] = m_key;
        json["value"] = m_value;
    }

    tablekey_string_node(node_ptr_t key, node_ptr_t value)
        : base_node{class_type}, m_key{std::move(key)}, m_value{std::move(value)}
    {
    }

    ~tablekey_string_node() = default;

private:
    node_ptr_t m_key;
    node_ptr_t m_value;
};

class tablevalue_node : public base_node {
public:
    static constexpr node_type class_type = stmt_table_value;

    const node_ptr_t &value() const
    {
        return m_value;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["value"] = m_value;
    }

    tablevalue_node(node_ptr_t value)
        : base_node{class_type}, m_value{std::move(value)}
    {
    }

    ~tablevalue_node() = default;

private:
    node_ptr_t m_value;
};

} // namespace __detail

// exports
using tablekey_t = __detail::tablekey_node;
using tableval_t = __detail::tablevalue_node;
using tablekey_string_t = __detail::tablekey_string_node;
using table_construct_expr_t = __detail::table_construct_expr_node;

} // namespace ast

CPPLUA_NS_END
#endif

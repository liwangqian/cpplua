#ifndef CPPLUA_LOOP_H
#define CPPLUA_LOOP_H

#include "base.h"
#include <utility>
#include <vector>

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class fornumeric_node : public base_node {
public:
    static constexpr node_type class_type = stmt_for_numeric;

    const node_ptr_t &variable() const
    { return m_var; }

    const node_ptr_t &start() const
    { return m_start; }

    const node_ptr_t &end() const
    { return m_end; }

    const node_ptr_t &step() const
    { return m_step; }

    const std::vector<node_ptr_t> &body() const
    { return m_body; }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["variable"] = m_var;
        json["start"] = m_start;
        json["end"] = m_end;
        if (m_step) json["step"] = m_step;
        json["body"] = m_body;
    }

    fornumeric_node(node_ptr_t var, node_ptr_t start, node_ptr_t end, node_ptr_t step, std::vector<node_ptr_t> body)
        : base_node{class_type}, m_var{std::move(var)}, m_start{std::move(start)}, m_end{std::move(end)},
          m_step{std::move(step)}, m_body{std::move(body)}
    {
    }

    ~fornumeric_node() = default;

private:
    node_ptr_t m_var;
    node_ptr_t m_start;
    node_ptr_t m_end;
    node_ptr_t m_step;
    std::vector<node_ptr_t> m_body;
};

class forgeneric_node : public base_node {
public:
    static constexpr node_type class_type = stmt_for_generic;

    const std::vector<node_ptr_t> &variables() const
    { return m_vars; }

    const std::vector<node_ptr_t> &iters() const
    { return m_iters; }

    const std::vector<node_ptr_t> &body() const
    { return m_body; }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["variables"] = m_vars;
        json["iterators"] = m_iters;
        json["body"] = m_body;
    }

    forgeneric_node(std::vector<node_ptr_t> vars, std::vector<node_ptr_t> iters, std::vector<node_ptr_t> body)
        : base_node{class_type}, m_vars{std::move(vars)}, m_iters{std::move(iters)}, m_body{std::move(body)}
    {
    }

    ~forgeneric_node() = default;

private:
    std::vector<node_ptr_t> m_vars;
    std::vector<node_ptr_t> m_iters;
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using fornumeric_t = __detail::fornumeric_node;
using forgeneric_t = __detail::forgeneric_node;

} // namespace ast

CPPLUA_NS_END
#endif

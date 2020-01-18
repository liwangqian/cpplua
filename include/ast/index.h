#ifndef CPPLUA_INDEX_H
#define CPPLUA_INDEX_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class index_node : public base_node {
public:
    static constexpr node_type class_type = expr_index;

    const node_ptr_t &base() const
    {
        return m_base;
    }

    const node_ptr_t &indexer() const
    {
        return m_indexer;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["base"] = m_base;
        json["indexer"] = m_indexer;
    }

    index_node(node_ptr_t base, node_ptr_t indexer)
        : base_node{class_type}, m_base{std::move(base)}, m_indexer{std::move(indexer)}
    {
    }

    ~index_node() = default;

private:
    node_ptr_t m_base;
    node_ptr_t m_indexer;
};

} // namespace __detail

// exports
using index_t = __detail::index_node;

} // namespace ast

CPPLUA_NS_END
#endif

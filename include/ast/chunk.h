#ifndef CPPLUA_CHUNK_H
#define CPPLUA_CHUNK_H

#include <utility>

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class chunk_node : public base_node {
public:
    static constexpr node_type class_type = stmt_chunk;

    const std::vector<node_ptr_t> &body() const
    {
        return m_body;
    }

    void to_json(nlohmann::json &json) const override
    {
        base_node::to_json(json);
        json["body"] = m_body;
    }

    chunk_node(std::vector<node_ptr_t> body)
        : base_node{class_type}, m_body{std::move(body)}
    {
    }

    ~chunk_node() = default;
private:
    std::vector<node_ptr_t> m_body;
};

} // namespace __detail

// exports
using chunk_t = __detail::chunk_node;

} // namespace ast

CPPLUA_NS_END
#endif

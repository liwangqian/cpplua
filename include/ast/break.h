#ifndef CPPLUA_BREAK_H
#define CPPLUA_BREAK_H

#include "base.h"

CPPLUA_NS_BEGIN

namespace ast {
namespace __detail {

class break_node : public base_node {
public:
    static constexpr node_type class_type = stmt_break;

    explicit break_node()
        : base_node{class_type}
    {
    }

    ~break_node() = default;
};

} // namespace __detail

// exports
using break_t = __detail::break_node;

} // namespace ast

CPPLUA_NS_END
#endif

#ifndef CPPLUA_MARKER_H
#define CPPLUA_MARKER_H

#include "def.h"
#include "token.h"
#include "ast.h"
#include "range.h"

CPPLUA_NS_BEGIN

namespace __detail {

class marker {
public:
    inline void complete(const token_t &tok)
    {
        m_range.end = tok.range().end;
    }

    inline void bless(ast::node_ptr_t &node)
    {
        node->range = m_range;
    }

    marker(const token_t &tok)
        : m_range{tok.range()}
    {
    }

    marker() = delete;

    ~marker() = default;

private:
    range_t m_range;
};

} // namespace __detail

// exports
using marker_t = __detail::marker;

CPPLUA_NS_END

#endif

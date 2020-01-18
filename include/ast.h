#ifndef CPPLUA_AST_H
#define CPPLUA_AST_H

#include "ast/base.h"
#include "ast/chunk.h"
#include "ast/label.h"
#include "ast/local.h"
#include "ast/ident.h"
#include "ast/unary.h"
#include "ast/cond.h"
#include "ast/dostmt.h"
#include "ast/while.h"
#include "ast/goto.h"
#include "ast/repeat.h"
#include "ast/break.h"
#include "ast/return.h"
#include "ast/member.h"
#include "ast/func.h"
#include "ast/loop.h"
#include "ast/assign.h"
#include "ast/call.h"
#include "ast/binary.h"
#include "ast/index.h"
#include "ast/literal.h"
#include "ast/table.h"

CPPLUA_NS_BEGIN

namespace ast {

template<typename NodeType, typename ...Args>
node_ptr_t make_node(Args... argi)
{
    return std::make_shared<NodeType>(argi...);
}

} // namespace ast

CPPLUA_NS_END

#endif // CPPLUA_AST_H

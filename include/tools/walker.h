//
// Created by busted on 2020/2/29.
//

#ifndef CPPLUA_WALKER_H
#define CPPLUA_WALKER_H

#include "def.h"
#include "ast.h"
#include "env.h"
#include "symbol.h"

CPPLUA_NS_BEGIN
namespace tools {

struct walker_t {
    walker_t(const file_t *file, const ast::node_ptr_t &root);
    ~walker_t() = default;

    void go();
private:
    const file_t *file{nullptr};
    const ast::node_ptr_t &root;
    env_t *top{nullptr};
    func_t *fsym{nullptr};
    const name_t* anoynomous{nullptr};

    void enter_scope(env_t *env = nullptr);
    void exit_scope();

    symbol_t *parse_stmt_literal(const ast::node_ptr_t &node) const;
    symbol_t *create_symbol_from_rvalue(symbol_t *rv, name_t* name, const range_t& r) const;
    symbol_t *get_symbol_from_list(const std::vector<symbol_t*> &list, std::size_t i);

    void walk_nodes(const std::vector<ast::node_ptr_t> &nodes);
    symbol_t *walk_node(const ast::node_ptr_t& node);
    symbol_t *parse_chunk(const ast::node_ptr_t& node);
    symbol_t *parse_stmt_local(const ast::node_ptr_t& node);
    symbol_t *parse_stmt_func(const ast::node_ptr_t& node);
    symbol_t *parse_table_constructor(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_ident(const ast::node_ptr_t &node);
    symbol_t *parse_expr_binary(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_return(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_if(const ast::node_ptr_t &node);
    symbol_t *parse_clause_if(const ast::node_ptr_t &node);
    symbol_t *parse_clause_elseif(const ast::node_ptr_t &node);
    symbol_t *parse_clause_else(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_do(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_while(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_repeat(const ast::node_ptr_t &node);
    symbol_t *parse_stmt_assignment(const ast::node_ptr_t &node);
    symbol_t *parse_expr_call(const ast::node_ptr_t &node);
    symbol_t *parse_expr_call_1p(const ast::node_ptr_t &node);
    symbol_t *parse_expr_tablecall(const ast::node_ptr_t &node);
    symbol_t *parse_expr_stringcall(const ast::node_ptr_t &node);
    symbol_t *parse_expr_unary(const ast::node_ptr_t &node);
};

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_WALKER_H

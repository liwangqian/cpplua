#ifndef CPPLUA_PARSER_H
#define CPPLUA_PARSER_H

#include "def.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "marker.h"
#include "scope.h"
#include "error.h"
#include <vector>
#include <stack>

CPPLUA_NS_BEGIN

namespace __detail {

struct parser_config_t {
    const char *lua_version;
};

using ast_node_t = ast::node_ptr_t;
using marker_ptr_t = std::shared_ptr<marker_t>;

class parser {
public:
    parser(const parser_config_t &config, const char *input, uint32_t len);
    ~parser() = default;

    ast_node_t parse();

private:
    void next();
    bool consume(const char *value);
    void expect(const char *value);

    void validate_var(const ast_node_t &var) const;

    void start_node();
    ast_node_t finish_node(ast_node_t node);
    void create_scope();
    void destroy_scope();
    void attach_scope(ast_node_t &node, bool is_local);
    bool scope_has_name(const string_t &name);
    void scope_ident_name(const string_t &name);
    void scope_ident(ast_node_t &node);
    void ensure_eof();
    void check_body(const std::vector<ast_node_t> &body);

    ast_node_t parse_chunk();
    std::vector<ast_node_t> parse_block();

    ast_node_t parse_stmt();
    ast_node_t parse_local_stmt();
    ast_node_t parse_label_stmt();
    ast_node_t parse_if_stmt();
    ast_node_t parse_do_stmt();
    ast_node_t parse_for_stmt();
    ast_node_t parse_goto_stmt();
    ast_node_t parse_while_stmt();
    ast_node_t parse_break_stmt();
    ast_node_t parse_repeat_stmt();
    ast_node_t parse_return_stmt();
    ast_node_t parse_function_stmt();
    ast_node_t parse_table_constructor();
    ast_node_t parse_assignment_or_call_stmt();

    ast_node_t parse_ident();
    ast_node_t parse_expr();
    ast_node_t parse_sub_expr(int min_precedence);
    ast_node_t parse_expect_expr();
    ast_node_t parse_prefix_expr();
    ast_node_t parse_primary_expr();
    ast_node_t parse_call_expr(ast_node_t &base);

    ast_node_t parse_function_name();
    ast_node_t parse_function_decl(const ast_node_t &name, bool is_local);

    bool is_block_follow(const token_t &token) const;
    bool is_unary_op(const token_t &token) const;
    bool is_call_expr(const ast_node_t &expr) const;
    int binary_precedence(const string_t &op) const;

    marker_ptr_t create_location_marker();
    void complete_location_marker(ast_node_t &node);
    void pop_location_marker();

    ast_node_t raise(const error::syntax_error &error) const noexcept(false);
    ast_node_t unexpected(const token_t &tok) const;
    ast_node_t raise_unexpected_token(const string_t &desc, const token_t &tok) const;

    std::stack<marker_ptr_t> m_locations;
    std::stack<scope_t<string_t> *> m_scopes;
    stack_t<string_t> m_local_stack;
    token_t m_token;
    token_t m_prev;
    token_t m_lookahead;
    lexer_t m_lexer;
};

} // namespace __detail

// exports
using parser_t = __detail::parser;

CPPLUA_NS_END


#endif

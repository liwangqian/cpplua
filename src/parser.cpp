#include "parser.h"
#include "error.h"

CPPLUA_NS_BEGIN

namespace __detail {
using namespace ast;

parser::parser(const parser_config_t &config, const char *input, uint32_t len)
    : m_lexer{make_feature(config.lua_version), input, len}
{
    // nop
}

void parser::next()
{
    m_prev = m_token;
    m_token = m_lookahead;
    do {
        m_lookahead = m_lexer.lex();
    } while (m_lookahead.is_comment()); // skip comments
}

bool parser::consume(const char *value)
{
    if (m_token.value() == value) {
        next();
        return true;
    } else {
        return false;
    }
}

void parser::expect(const char *value)
{
    if (m_token.value() == value) {
        next();
    } else {
        auto pos = m_lexer.lineinfo().to_position(m_token.range().start);
        raise(error::expected{pos, value, m_token.value()});
    }
}

ast_node_t parser::parse()
{
    m_lookahead = m_lexer.lex();
    return parse_chunk();
}

void parser::start_node()
{
    m_locations.push(create_location_marker());
}

ast_node_t parser::finish_node(ast_node_t node)
{
    complete_location_marker(node);
    return node;
}

void parser::create_scope()
{
    m_scopes.push(new scope_t<string_t>{m_local_stack});
}

void parser::destroy_scope()
{
    delete m_scopes.top();
    m_scopes.pop();
}

void parser::attach_scope(ast_node_t &node, bool is_local)
{
    node->is_local = is_local;
}

bool parser::scope_has_name(const string_t &name)
{
    return m_local_stack.contains(name);
}

void parser::scope_ident_name(const string_t &name)
{
    if (scope_has_name(name)) return;
    m_local_stack.push(name);
}

void parser::scope_ident(ast_node_t &node)
{
    auto &ident = node->cast_to<ident_t>();
    scope_ident_name(ident.name());
    attach_scope(node, true);
}

void parser::ensure_eof()
{
    if (!m_token.is_eof()) unexpected(m_token);
}

void parser::check_body(const std::vector<ast_node_t> &body)
{
    if (body.empty()) m_prev = m_token;
}

ast_node_t parser::parse_chunk()
{
    next();
    start_node();
    create_scope();
    auto body = parse_block();
    destroy_scope();
    ensure_eof();
    check_body(body);
    return finish_node(make_node<chunk_t>(std::move(body)));
}

std::vector<ast_node_t> parser::parse_block()
{
    std::vector<ast_node_t> block;
    while (!is_block_follow(m_token)) {
        if (m_token.is_comment()) {
            next();
            continue;
        }
        if (m_token.value() == "return") {
            block.push_back(parse_stmt());
            break;
        }
        auto stmt = parse_stmt();
        consume(";");
        if (stmt) block.push_back(std::move(stmt));
    }

    return std::move(block);
}

ast_node_t parser::parse_stmt()
{
    start_node();

    if (m_token.is_keyword()) {
        switch (m_token.keyword_id()) {
            case KW_LOCAL:
                next();
                return parse_local_stmt();
            case KW_IF:
                next();
                return parse_if_stmt();
            case KW_RETURN:
                next();
                return parse_return_stmt();
            case KW_FUNCTION:
                next();
                return parse_function_stmt();
            case KW_WHILE:
                next();
                return parse_while_stmt();
            case KW_FOR:
                next();
                return parse_for_stmt();
            case KW_REPEAT:
                next();
                return parse_repeat_stmt();
            case KW_BREAK:
                next();
                return parse_break_stmt();
            case KW_DO:
                next();
                return parse_do_stmt();
            case KW_GOTO:
                next();
                return parse_goto_stmt();
            default:
                break;
        }
    }

    if (m_token.is_punct()) {
        if (consume("::"))
            return parse_label_stmt();
    }

    pop_location_marker();

    return parse_assignment_or_call_stmt();
}

ast_node_t parser::parse_local_stmt()
{
    if (m_token.is_ident()) {
        std::vector<node_ptr_t> vars{};
        std::vector<node_ptr_t> init{};

        do {
            vars.push_back(parse_ident());
        } while (consume(","));

        if (consume("=")) {
            do {
                init.push_back(parse_expect_expr());
            } while (consume(","));
        }

        return finish_node(make_node<local_t>(std::move(vars), std::move(init)));
    }
    if (consume("function")) {
        auto name = parse_ident();
        scope_ident(name);
        create_scope();
        return parse_function_decl(name, true);
    }
    return raise_unexpected_token("<name>", m_token);
}

ast_node_t parser::parse_label_stmt()
{
    auto label = parse_ident();
    expect("::");
    return finish_node(make_node<label_t>(label));
}

ast_node_t parser::parse_assignment_or_call_stmt()
{
    auto prev = m_token;
    auto marker = create_location_marker();
    auto expr = parse_prefix_expr();
    if (!expr) return unexpected(m_token);
    if (m_token.value() == "," || m_token.value() == "=") {
        std::vector<ast_node_t> vars{expr};
        std::vector<ast_node_t> inits;
        validate_var(expr);
        while (consume(",")) {
            expr = parse_expect_expr();
            if (!expr) return raise_unexpected_token("<expression>", m_token);
            validate_var(expr);
            vars.push_back(expr);
        }
        expect("=");
        do {
            expr = parse_expect_expr();
            inits.push_back(expr);
        } while (consume(","));

        m_locations.push(marker);
        return finish_node(make_node<assignment_t>(std::move(vars), std::move(inits)));
    }
    if (is_call_expr(expr)) {
        m_locations.push(marker);
        return finish_node(make_node<callstmt_t>(expr));
    }
    return unexpected(prev);
}

ast_node_t parser::parse_ident()
{
    auto tok = m_token;
    if (!tok.is_ident()) return raise_unexpected_token("<name>", tok);
    start_node();
    next();
    return finish_node(make_node<ident_t>(tok.value()));
}

ast_node_t parser::parse_expect_expr()
{
    auto expr = parse_expr();
    if (!expr) return raise_unexpected_token("<expression>", m_token);
    return expr;
}

ast_node_t parser::parse_expr()
{
    return parse_sub_expr(0);
}

ast_node_t parser::parse_sub_expr(int min_precedence)
{
    auto tok = m_token;
    ast_node_t expr;

    auto marker = create_location_marker();

    if (is_unary_op(tok)) {
        start_node();
        next();
        auto arg = parse_sub_expr(10);
        if (!arg) return raise_unexpected_token("<expression>", m_token);
        expr = finish_node(make_node<unary_t>(tok.value(), arg));
    }
    if (!expr) {
        expr = parse_primary_expr();
        if (!expr) expr = parse_prefix_expr();
    }

    if (!expr) return nullptr;

    int precedence;
    while (true) {
        auto op = m_token.value();
        precedence = (m_token.is_punct() || m_token.is_keyword()) ?
                     binary_precedence(op) : 0;

        if (precedence == 0 || precedence <= min_precedence) break;
        if (op == "^" || op == "..") --precedence;
        next();
        auto right = parse_sub_expr(precedence);
        if (!right) return raise_unexpected_token("<expression>", m_token);
        m_locations.push(marker);
        expr = finish_node(make_node<binaryop_t>(op, expr, right));
    }
    return expr;
}

ast_node_t parser::parse_primary_expr()
{
    auto tok = m_token;
    auto marker = create_location_marker();
    if (tok.is_literal()) {
        m_locations.push(marker);
        next();
        return finish_node(make_node<literal_t>(tok.type(), tok.value()));
    } else if (tok.is_keyword() && tok.keyword_id() == KW_FUNCTION) {
        m_locations.push(marker);
        next();
        create_scope();
        return parse_function_decl(nullptr, false);
    } else if (consume("{")) {
        m_locations.push(marker);
        return parse_table_constructor();
    } else {
        return nullptr;
    }

}

ast_node_t parser::parse_prefix_expr()
{
    auto marker = create_location_marker();
    ast_node_t base;
    if (m_token.is_ident()) {
        auto name = m_token.value();
        base = parse_ident();
        attach_scope(base, scope_has_name(name));
    } else if (consume("(")) {
        base = parse_expect_expr();
        expect(")");
    } else {
        return nullptr;
    }

    ast_node_t expr, ident;
    while (true) {
        if (m_token.is_punct()) {
            if (m_token.value().size() >= 2) return base;
            auto punct = m_token.value();
            switch (punct[0]) {
                case '[':
                    m_locations.push(marker);
                    next();
                    expr = parse_expect_expr();
                    expect("]");
                    base = finish_node(make_node<index_t>(base, expr));
                    break;
                case '.':
                    m_locations.push(marker);
                    next();
                    ident = parse_ident();
                    base = finish_node(make_node<member_t>(base, punct, ident));
                    break;
                case ':':
                    m_locations.push(marker);
                    next();
                    ident = parse_ident();
                    base = finish_node(make_node<member_t>(base, punct, ident));
                    // pass through
                case '(':
                case '{':
                    m_locations.push(marker);
                    base = parse_call_expr(base);
                    break;
                default:
                    return base;
            }
        } else if (m_token.is_string()) {
            m_locations.push(marker);
            base = parse_call_expr(base);
        } else {
            break;
        }
    }
    return base;
}

ast_node_t parser::parse_if_stmt()
{
    m_locations.push(m_locations.top());
    auto condition = parse_expect_expr();
    expect("then");
    create_scope();
    auto body = parse_block();
    destroy_scope();

    auto node = make_node<ifstmt_t>();
    auto &ifstmt = node->cast_to<ifstmt_t>();

    ifstmt.add_clause(finish_node(make_node<ifclause_t>(condition, std::move(body))));
    while (consume("elseif")) {
        auto marker = create_location_marker();
        m_locations.push(marker);
        condition = parse_expect_expr();
        expect("then");
        create_scope();
        body = parse_block();
        destroy_scope();
        ifstmt.add_clause(finish_node(make_node<elseifclause_t>(condition, std::move(body))));
    }

    if (consume("else")) {
        m_locations.push(std::make_shared<marker_t>(m_prev));
        create_scope();
        body = parse_block();
        destroy_scope();
        ifstmt.add_clause(finish_node(make_node<elseclause_t>(std::move(body))));
    }

    expect("end");
    return finish_node(node);
}

ast_node_t parser::parse_do_stmt()
{
    create_scope();
    auto body = parse_block();
    destroy_scope();
    expect("end");
    return finish_node(make_node<dostmt_t>(std::move(body)));
}

ast_node_t parser::parse_for_stmt()
{
    auto var = parse_ident();
    create_scope();
    scope_ident(var);

    if (consume("=")) {
        auto start = parse_expect_expr();
        expect(",");
        auto end = parse_expect_expr();
        auto step = consume(",") ? parse_expect_expr() : nullptr;

        expect("do");
        auto body = parse_block();
        expect("end");
        destroy_scope();
        return finish_node(make_node<fornumeric_t>(var, start, end, step, std::move(body)));
    } else {
        std::vector<ast_node_t> vars{var};
        while (consume(",")) {
            var = parse_ident();
            scope_ident(var);
            vars.push_back(var);
        }
        expect("in");

        std::vector<ast_node_t> iters;
        do {
            auto expr = parse_expect_expr();
            iters.push_back(expr);
        } while (consume(","));

        expect("do");
        auto body = parse_block();
        expect("end");
        destroy_scope();
        return finish_node(make_node<forgeneric_t>(std::move(vars), std::move(iters), std::move(body)));
    }
}

ast_node_t parser::parse_goto_stmt()
{
    auto label = parse_ident();
    return finish_node(make_node<gotostmt_t>(label));
}

ast_node_t parser::parse_while_stmt()
{
    auto condition = parse_expect_expr();
    expect("do");
    create_scope();
    auto body = parse_block();
    destroy_scope();
    expect("end");
    return finish_node(make_node<whilestmt_t>(condition, std::move(body)));
}

ast_node_t parser::parse_break_stmt()
{
    return finish_node(make_node<break_t>());
}

ast_node_t parser::parse_repeat_stmt()
{
    create_scope();
    auto body = parse_block();
    expect("until");
    auto condition = parse_expect_expr();
    destroy_scope();
    return finish_node(make_node<repeat_t>(condition, std::move(body)));
}

ast_node_t parser::parse_return_stmt()
{
    auto node = make_node<returnstmt_t>();
    auto &stmt = node->cast_to<returnstmt_t>();
    if (m_token.value() != "end") {
        auto expr = parse_expr();
        if (expr) stmt.push(expr);
        while (consume(",")) {
            expr = parse_expect_expr();
            stmt.push(expr);
        }
        consume(";");
    }
    return finish_node(node);
}

ast_node_t parser::parse_function_stmt()
{
    auto name = parse_function_name();
    return parse_function_decl(name, false);
}

ast_node_t parser::parse_function_name()
{
    auto marker = create_location_marker();
    auto base = parse_ident();
    auto &ident = base->cast_to<ident_t>();
    attach_scope(base, scope_has_name(ident.name()));
    create_scope();

    while (consume(".")) {
        m_locations.push(marker);
        base = finish_node(make_node<member_t>(base, ".", parse_ident()));
    }

    if (consume(":")) {
        m_locations.push(marker);
        base = finish_node(make_node<member_t>(base, ":", parse_ident()));
        scope_ident_name("self");
    }

    return base;
}

ast_node_t parser::parse_table_constructor()
{
    std::vector<ast_node_t> fields;
    while (true) {
        start_node();
        if (m_token.is_punct() && consume("[")) {
            auto key = parse_expect_expr();
            expect("]");
            expect("=");
            auto value = parse_expect_expr();
            fields.push_back(finish_node(make_node<tablekey_t>(key, value)));
        } else if (m_token.is_ident()) {
            if (m_lookahead.value() == "=") {
                auto key = parse_ident();
                next();
                auto value = parse_expect_expr();
                fields.push_back(finish_node(make_node<tablekey_string_t>(key, value)));
            } else {
                auto value = parse_expect_expr();
                fields.push_back(finish_node(make_node<tableval_t>(value)));
            }
        } else {
            auto value = parse_expr();
            if (!value) {
                m_locations.pop();
                break;
            }
            fields.push_back(finish_node(make_node<tableval_t>(value)));
        }

        if (m_token.value() == "," || m_token.value() == ";") {
            next();
            continue;
        }
        break;
    }
    expect("}");
    return finish_node(make_node<table_construct_expr_t>(std::move(fields)));
}

ast_node_t parser::parse_function_decl(const ast_node_t &name, bool is_local)
{
    std::vector<ast_node_t> params;
    expect("(");
    if (!consume(")")) {
        while (true) {
            if (m_token.is_ident()) {
                auto param = parse_ident();
                scope_ident(param);
                params.push_back(param);
                if (consume(",")) continue;
                else if (consume(")")) break;
            } else if (m_token.is_vararg()) {
                params.push_back(parse_primary_expr());
                expect(")");
                break;
            } else {
                auto pos = m_lexer.lineinfo().to_position(m_token.range().start);
                raise(error::unexpected_token{pos, "<name> or '...'", m_token.value()});
            }
        }
    }
    auto body = parse_block();
    expect("end");
    destroy_scope();
    return finish_node(make_node<funcdecl_t>(name, std::move(params), std::move(body), is_local));
}

ast_node_t parser::parse_call_expr(ast_node_t &base)
{
    if (m_token.is_punct()) {
        auto c = m_token.value()[0];
        switch (c) {
            case '(': {
                next();
                std::vector<ast_node_t> exprs;
                auto expr = parse_expr();
                if (expr) exprs.push_back(expr);
                while (consume(",")) {
                    expr = parse_expect_expr();
                    exprs.push_back(expr);
                }
                expect(")");
                return finish_node(make_node<callexpr_t>(base, std::move(exprs)));
            }
            case '{': {
                start_node();
                next();
                auto table = parse_table_constructor();
                return finish_node(make_node<table_call_t>(base, table));
            }
            default:
                break;
        }
    } else if (m_token.is_string()) {
        return finish_node(make_node<string_call_t>(base, parse_primary_expr()));
    }

    return raise_unexpected_token("function arguments", m_token);
}

//////////////////////////////////

bool parser::is_block_follow(const token_t &token) const
{
    if (token.is_eof())
        return true;
    if (!token.is_keyword())
        return false;
    return token.keyword_id() == KW_ELSE ||
           token.keyword_id() == KW_ELSEIF ||
           token.keyword_id() == KW_END ||
           token.keyword_id() == KW_UNTIL;
}

bool parser::is_unary_op(const token_t &token) const
{
    const string_t &value = token.value();
    if (token.is_punct() && value.size() == 1) {
        return value[0] == '#' || value[0] == '-' || value[0] == '~';
    }
    return token.is_keyword() && value == "not";
}

bool parser::is_call_expr(const ast_node_t &expr) const
{
    switch (expr->type) {
        case node_type_t::expr_call:
        case node_type_t::expr_table_call:
        case node_type_t::expr_string_call:
            return true;
        default:
            return false;
    }
}

marker_ptr_t parser::create_location_marker()
{
    return std::make_shared<marker_t>(m_token);
}

void parser::complete_location_marker(ast_node_t &node)
{
    auto location = m_locations.top();
    location->complete(m_prev);
    location->bless(node);
    m_locations.pop();
}

void parser::pop_location_marker()
{
    auto location = m_locations.top();
    m_locations.pop();
}

void parser::validate_var(const ast_node_t &var) const
{
}

ast_node_t parser::unexpected(const token_t &tok) const
{
    const auto &near_ = m_lookahead.value();
    auto pos = m_lexer.lineinfo().to_position(tok.range().start);
    if (tok.type() != Invalid) {
        const char *type = nullptr;
        switch (tok.type()) {
            case StringLiteral:
                type = "string";
                break;
            case Keyword:
                type = "keyword";
                break;
            case Identifier:
                type = "identifier";
                break;
            case NumericLiteral:
                type = "number";
                break;
            case Punctuator:
                type = "symbol";
                break;
            case BooleanLiteral:
                type = "boolean";
                break;
            default:
                return raise(error::unexpected{pos, "symbol", tok.value(), near_});
        }
        return raise(error::unexpected{pos, type, tok.value(), near_});
    }
    return raise(error::unexpected{pos, "symbol", tok.value(), near_});
}

ast_node_t parser::raise_unexpected_token(const string_t &desc, const token_t &tok) const
{
    auto pos = m_lexer.lineinfo().to_position(m_token.range().start);
    return raise(error::unexpected_token{pos, desc, tok.value()});
}

ast_node_t parser::raise(const error::syntax_error &error) const noexcept(false)
{
    throw error;
}

/* 计算二元运算符的优先级 */
int parser::binary_precedence(const string_t &op) const
{
    if (op.size() == 1) {
        switch (op[0]) {
            case '^':
                return 12;
            case '*':
            case '/':
            case '%':
                return 10;
            case '+':
            case '-':
                return 9;
            case '&':
                return 6;
            case '~':
                return 5;
            case '|':
                return 4;
            case '<':
            case '>':
                return 3;
            default:
                return 0;
        }
    } else if (op.size() == 2) {
        auto c0 = op[0];
        auto c1 = op[1];
        if (c0 == c1) {
            switch (c0) {
                case '/':
                    return 10;
                case '.':
                    return 8;
                case '<':
                case '>':
                    return 7;
                case '=':
                    return 3;
                default:
                    return 0;
            }
        } else {
            if ((c0 == '<' || c0 == '>' || c0 == '~') && c1 == '=') return 3;
            if (c0 == 'o' && c1 == 'r') return 1;
        }
    } else {
        if (op[0] == 'a' && op[1] == 'n' && op[2] == 'd') return 2;
    }
    return 0;
}

} // namespace __detail
CPPLUA_NS_END

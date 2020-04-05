//
// Created by busted on 2020/3/8.
//

#include <iostream>
#include "walker.h"
#include "names.h"
#include "files.h"
#include "symbol.h"

CPPLUA_NS_BEGIN
namespace tools {

walker_t::walker_t(const file_t *file, const ast::node_ptr_t &root)
    : file{file}, root{root}
{
}

void walker_t::go()
{
    walk_node(root);
}

void walker_t::enter_scope(env_t *env)
{
    top = env ? env : new env_t{top};
}

void walker_t::exit_scope()
{
    string_t spaces(top->depth() * 2, ' ');
    top->foreach([&spaces](symbol_t *s) {
//        std::cout << s->location.range.to_string();
        std::cout << spaces << s->to_string() << std::endl;
    });
    std::cout << "---------" << std::endl;
    top = top->prev_env();
}

void walker_t::walk_nodes(const std::vector<ast::node_ptr_t> &nodes)
{
    for (auto &node : nodes) {
        walk_node(node);
    }
}

symbol_t *walker_t::walk_node(const ast::node_ptr_t &node)
{
    switch (node->type) {
        case ast::node_type_t::stmt_chunk:
            return parse_chunk(node);
        case ast::node_type_t::stmt_ident:
            return parse_stmt_ident(node);
        case ast::node_type_t::stmt_local:
            return parse_stmt_local(node);
        case ast::node_type_t::stmt_return:
            return parse_stmt_return(node);
        case ast::node_type_t::stmt_if:
            return parse_stmt_if(node);
        case ast::node_type_t::stmt_if_clause:
            return parse_clause_if(node);
        case ast::node_type_t::stmt_elseif_clause:
            return parse_clause_elseif(node);
        case ast::node_type_t::stmt_else_clause:
            return parse_clause_else(node);
        case ast::node_type_t::stmt_do:
            return parse_stmt_do(node);
        case ast::node_type_t::stmt_while:
            return parse_stmt_while(node);
        case ast::node_type_t::stmt_repeat:
            return parse_stmt_repeat(node);
        case ast::node_type_t::stmt_assignment:
            return parse_stmt_assignment(node);
        case ast::node_type_t::stmt_call:
            return parse_stmt_call(node);
        case ast::node_type_t::stmt_function:
            return parse_stmt_func(node);
        case ast::node_type_t::stmt_for_numeric:
            return parse_stmt_fori(node);
        case ast::node_type_t::stmt_for_generic:
            return parse_stmt_for(node);
        case ast::node_type_t::stmt_literal:
            return parse_stmt_literal(node);
        case ast::node_type_t::stmt_table_key:
            return parse_stmt_tkey(node);
        case ast::node_type_t::stmt_table_value:
            return parse_stmt_tval(node);
        case ast::node_type_t::expr_table_constructor:
            return parse_table_ctor(node);
        case ast::node_type_t::expr_binary:
            return parse_expr_binary(node);
        case ast::node_type_t::expr_unary:
            return parse_expr_unary(node);
        case ast::node_type_t::expr_member:
            return parse_expr_member(node);
        case ast::node_type_t::expr_index:
            return parse_expr_index(node);
        case ast::node_type_t::expr_call:
            return parse_expr_call(node);
        case ast::node_type_t::expr_table_call:
            return parse_expr_tcall(node);
        case ast::node_type_t::expr_string_call:
            return parse_expr_scall(node);
        case ast::node_type_t::comment:
            break;
        default:
            break;
    }
}

symbol_t *walker_t::parse_chunk(const ast::node_ptr_t &node)
{
    enter_scope();
    const auto &chunk = node->cast_to<ast::chunk_t>();
    for (const auto &item : chunk.body()) {
        walk_node(item);
    }
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::get_symbol_from_list(const std::vector<symbol_t*> &list, std::size_t i)
{
    if (list.empty()) return nullptr;
    if ((i >= (list.size() - 1)) && is_group(list.back())) {
        const auto size = list.size() - 1;
        const auto offset = i - size;
        const auto group = dynamic_cast<const symbol_group_t*>(list.back());
        if (offset < group->symbols.size()) {
            return create_symbol<ref_t>(nullptr, top, group->symbols[offset]);
        } else {
            return nullptr;
        }
    } else {
        if (i < list.size()) {
            return list[i];
        }
        return nullptr;
    }
}

symbol_t *walker_t::parse_stmt_literal(const ast::node_ptr_t &node) const
{
    const auto &literal = node->cast_to<ast::literal_t>();
    switch (literal.value_type()) {
        case token_type_t::StringLiteral:
            return create_symbol<str_t>();
        case token_type_t::BooleanLiteral:
            return create_symbol<boolean_t>();
        case token_type_t::NumericLiteral:
            return create_symbol<number_t>();
        default:
            return create_symbol<any_t>();
    }
}

symbol_t *walker_t::parse_stmt_local(const ast::node_ptr_t &node)
{
    // local name = number | boolean | string | nil | function | table | expr
    const auto &local = node->cast_to<ast::local_t>();
    return parse_expr_assignment(local.variables(), local.init(), true);
}

symbol_t *walker_t::parse_expr_assignment(const std::vector<ast::node_ptr_t> &vars,
    const std::vector<ast::node_ptr_t> &inits, bool is_local)
{
    std::vector<symbol_t *> rhs;
    if (!inits.empty()) {
        rhs.reserve(inits.size());
        for (const auto & init : inits) {
            auto li = walk_node(init);
            if (li != nullptr) rhs.push_back(li);
        }
    }

    for (std::size_t i = 0; i < vars.size(); ++i) {
        auto var = &vars[i];
        table_t *t = nullptr;
        if ((*var)->type == ast::node_type_t::expr_member) {
            const auto &expr = (*var)->cast_to<ast::member_t>();
            std::vector<name_t*> bases;
            get_names_from_ident(expr.base(), bases);
            auto parent = find_parent_by_name(bases);
            if (parent == nullptr) {
                continue;
            }
            if (is_ref(parent)) {
                auto def = parent->cast_to_ptr<ref_t*>()->def();
                if (!def || !is_table(def)) {
                    continue;
                }
                parent = def;
            }
            if (!is_table(parent)) {
                continue;
            }

            t = parent->cast_to_ptr<table_t*>();
            var = &expr.identifier();
        }
        if ((*var)->type != ast::node_type_t::stmt_ident) {
            continue;
        }

        const auto &ident = (*var)->cast_to<ast::ident_t>();;
        if (ident.name() == "_") {
            continue;
        }

        const name_t *name = resolve_name(ident.name());
        symbol_t *lhs = nullptr;
        auto rv = get_symbol_from_list(rhs, i);
        if (rv != nullptr) {
            lhs = create_symbol_from_rvalue(rv, name, ident.range);
        } else {
            lhs = create_symbol<any_t>(name, top);
            lhs->location.file = file;
            lhs->location.range = ident.range;
            top->put(name->value, lhs);
        }

        if (lhs != nullptr) {
            lhs->is_local = is_local;
        }

        if (t != nullptr) {
            t->fields.push_back(lhs);
        }
    }
    return nullptr;
}

symbol_t *walker_t::parse_stmt_assignment(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::assignment_t>();
    return parse_expr_assignment(stmt.vars(), stmt.inits(), false);
}

void walker_t::get_names_from_ident(const ast::node_ptr_t &node, std::vector<name_t *> &names)
{
    if (!node) {
        return;
    }

    switch (node->type) {
        case ast::node_type_t::stmt_ident: {
            auto &ident = node->cast_to<ast::ident_t>();
            names.push_back(resolve_name(ident.name()));
            return;
        }
        case ast::node_type_t::expr_member: {
            auto &member = node->cast_to<ast::member_t>();
            get_names_from_ident(member.base(), names);
            get_names_from_ident(member.identifier(), names);
            return;
        }
        default: return;
    }
}

symbol_t *walker_t::parse_stmt_func(const ast::node_ptr_t &node)
{
    const auto &func = node->cast_to<ast::funcdecl_t>();

    std::vector<name_t*> names;
    get_names_from_ident(func.identifier(), names);
    const auto fname = names.empty() ? nullptr : names.back();
    auto fs = create_symbol<func_t>(fname, top);
    fs->location.file = file;
    fs->location.range = node->range;
    fs->is_local = node->is_local;

    table_t *t = nullptr;
    if (names.size() == 1) { // [local?] function xx() end;
        if (fname) top->put(fname->value, fs);
    } else if (names.size() > 1) { // function M.xx() end;
        std::vector<name_t*> bases{names.begin(), names.end() - 1};
        auto parent = find_parent_by_name(bases);
        if (parent && is_table(parent)) {
            t = parent->cast_to_ptr<table_t*>();
            t->fields.push_back(fs);
            // 绑定self到父表
            if (func.identifier()->cast_to<ast::member_t>().indexer() == ":") {
                auto self = create_symbol<ref_t>(resolve_name("self"), &fs->fenv, parent);
                fs->fenv.put("self", self);
            }
        }
    }

    enter_scope(&fs->fenv);
    for (const auto &param : func.params()) {
        const name_t *pname = nullptr;
        if (param->type == ast::node_type_t::stmt_ident) {
            const auto &ident = param->cast_to<ast::ident_t>();
            pname = resolve_name(ident.name());
        } else if (param->type == ast::node_type_t::stmt_literal) {
            const auto &literal = param->cast_to<ast::literal_t>();
            pname = resolve_name(literal.value());
        } else {
            continue;
        }
        auto fp = create_symbol<any_t>(pname, top);
        fp->is_local = true;
        fp->location.file = file;
        fp->location.range = param->range;
        top->put(pname->value, fp);
        fs->params.push_back(fp);
    }

    auto save_fsym = fsym;
    fsym = fs;
    walk_nodes(func.body());
    fsym = save_fsym;

    exit_scope();
    return fs;
}

symbol_t *walker_t::parse_table_ctor(const ast::node_ptr_t &node)
{
    using ast::node_type_t;
    const auto &table = node->cast_to<ast::table_construct_expr_t>();
    auto ts = create_symbol<table_t>(nullptr, top);
    ts->location.file = file;
    ts->location.range = table.range;
    ts->is_local = table.is_local;

    enter_scope();
    for (const auto &field : table.fields()) {
        if (field->type == node_type_t::stmt_table_key_string) {
            const auto &fn = field->cast_to<ast::tablekey_string_t>();
            auto f = walk_node(fn.value());
            if (f == nullptr) {
                f = create_symbol<any_t>();
            }
            std::vector<name_t *> names;
            get_names_from_ident(fn.key(), names);
            assert(names[0]);
            f->name = names[0];
            f->location.file = file;
            f->location.range = fn.key()->range;
            ts->fields.push_back(f);
        } else {
            walk_node(field);
        }
    }
    exit_scope();
    return ts;
}

symbol_t *walker_t::parse_stmt_ident(const ast::node_ptr_t &node)
{
    const auto &ident = node->cast_to<ast::ident_t>();
    auto def = top->get(ident.name());
    if (def) {
        def->references.emplace_front(location_t{file, ident.range});
        return def;
    }
    return create_symbol<undefined_t>(resolve_name(ident.name()), top);
}

symbol_t *walker_t::parse_expr_binary(const ast::node_ptr_t &node)
{
    const auto &expr = node->cast_to<ast::binaryop_t>();
    if (expr.oper().size() == 1) {
        switch (expr.oper()[0]) {
            case '+': case '-': case '*':
            case '/': case '^': case '%':
                walk_node(expr.right());
                return walk_node(expr.left());
            case '<': case '>':
                return create_symbol<boolean_t>();
            default: return nullptr;
        }
    }
    if (expr.oper() == "..") {
        return create_symbol<str_t>();
    }
    // "<=", ">=", "~=", "=="
    return create_symbol<boolean_t>();
}

symbol_t *walker_t::create_symbol_from_rvalue(symbol_t *rv, const name_t* name, const vrange_t &r) const
{
    switch (rv->type) {
        case symbol_type_t::any:
        case symbol_type_t::number:
        case symbol_type_t::string:
        case symbol_type_t::boolean:
        case symbol_type_t::table:
        case symbol_type_t::function: {
            if (rv->name == nullptr) {
                rv->name = name;
                rv->env = top;
                rv->env->put(name->value, rv);
                rv->location.file = file;
                rv->location.range = r;
                return rv;
            }
        } // fall thought
        case symbol_type_t::multi:
        case symbol_type_t::ref:
        case symbol_type_t::returns_of:
        case symbol_type_t::undefined: {
            auto s = create_symbol<ref_t>(name, top, rv);
            s->env->put(name->value, s);
            s->location.file = file;
            s->location.range = r;
            return s;
        }
        default:
            return nullptr;
    }
}

symbol_t *walker_t::parse_stmt_return(const ast::node_ptr_t &node)
{
    const auto &ret = node->cast_to<ast::returnstmt_t>();
    const auto esize = ret.expressions().size();
    if (fsym->returns.size() < esize) {
        fsym->returns.resize(esize);
    }

    for (std::size_t i = 0; i < esize; ++i) {
        auto &e = ret.expressions()[i];
        auto sn = walk_node(e);
        if (sn == nullptr) {
            sn = create_symbol<any_t>();
            sn->location.file = file;
            sn->location.range = e->range;
        }
        auto so = fsym->returns[i];
        if (so == nullptr) {
            fsym->returns[i] = sn;
            continue;
        }

        if (sn->type == so->type) {
            continue;
        }

        if (is_multi(so)) {
            auto sm = dynamic_cast<multi_type_t*>(so);
            bool has_same_type = false;
            for (auto &si : sm->symbols) {
                if (si->type == sn->type) {
                    has_same_type = true;
                    break;
                }
            }
            if (!has_same_type) {
                sm->symbols.push_back(sn);
            }
            continue;
        }

        fsym->returns[i] = create_symbol<multi_type_t>(so, sn);
    }
    return nullptr;
}

symbol_t *walker_t::parse_stmt_if(const ast::node_ptr_t &node)
{
    const auto &ifstmt = node->cast_to<ast::ifstmt_t>();
    walk_nodes(ifstmt.clauses());
    return nullptr;
}

symbol_t *walker_t::parse_clause_if(const ast::node_ptr_t &node)
{
    const auto &cl = node->cast_to<ast::ifclause_t>();
    enter_scope();
    walk_node(cl.condition());
    walk_nodes(cl.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_clause_elseif(const ast::node_ptr_t &node)
{
    const auto &cl = node->cast_to<ast::elseifclause_t>();
    enter_scope();
    walk_node(cl.condition());
    walk_nodes(cl.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_clause_else(const ast::node_ptr_t &node)
{
    const auto &cl = node->cast_to<ast::elseclause_t>();
    enter_scope();
    walk_nodes(cl.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_stmt_do(const ast::node_ptr_t &node)
{
    const auto &dostmt = node->cast_to<ast::dostmt_t>();
    enter_scope();
    walk_nodes(dostmt.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_stmt_while(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::whilestmt_t>();
    enter_scope();
    walk_node(stmt.condition());
    walk_nodes(stmt.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_stmt_repeat(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::repeat_t>();
    enter_scope();
    walk_node(stmt.condition());
    walk_nodes(stmt.body());
    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_expr_call(const ast::node_ptr_t &node)
{
    const auto &expr = node->cast_to<ast::callexpr_t>();
    walk_nodes(expr.args());
    auto fp = walk_node(expr.base()); //TODO: 肯定是function
    if (fp && is_function(fp)) {
        auto ff = dynamic_cast<func_t*>(fp);
        return create_symbol<symbol_group_t>(ff->returns);
    }
    return fp;
}

symbol_t *walker_t::parse_expr_tcall(const ast::node_ptr_t &node)
{
    return parse_expr_call_1p(node);
}

symbol_t *walker_t::parse_expr_scall(const ast::node_ptr_t &node)
{
    return parse_expr_call_1p(node);
}

symbol_t *walker_t::parse_expr_call_1p(const ast::node_ptr_t &node)
{
    // supper of table_call and string_call is the same.
    const auto &expr = dynamic_cast<ast::table_call_t::supper&>(*node.get());
    walk_node(expr.arg());
    auto fp = walk_node(expr.base());
    if (fp && is_function(fp)) {
        auto ff = dynamic_cast<func_t*>(fp);
        return create_symbol<symbol_group_t>(ff->returns);
    }
    return fp;
}

symbol_t *walker_t::parse_expr_unary(const ast::node_ptr_t &node)
{
    const auto &expr = node->cast_to<ast::unary_t>();
    if (expr.op() == "not") {
        return create_symbol<boolean_t>();
    }
    if (expr.op() == "#" || expr.op() == "-") {
        return create_symbol<number_t>();
    }
    return nullptr;
}

symbol_t *walker_t::parse_expr_index(const ast::node_ptr_t &node)
{
    const auto &expr = node->cast_to<ast::index_t>();
    walk_node(expr.base());
    walk_node(expr.indexer());
    return nullptr;
}

symbol_t *walker_t::parse_stmt_tkey(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::tablekey_t>();
    return walk_node(stmt.value());
}

symbol_t *walker_t::parse_stmt_tval(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::tableval_t>();
    return walk_node(stmt.value());
}

symbol_t *walker_t::parse_stmt_fori(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::fornumeric_t>();
    enter_scope();

    {
        const auto &var = stmt.variable()->cast_to<ast::ident_t>();
        if (var.name() != "_") {
            auto s = create_symbol<number_t>(resolve_name(var.name()), top);
            s->is_local = true;
            s->location.file = file;
            s->location.range = var.range;
            top->put(var.name(), s);
        }
    }

    {
        walk_node(stmt.start());
        walk_node(stmt.end());
        if (stmt.step()) {
            walk_node(stmt.step());
        }
    }

    walk_nodes(stmt.body());

    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_stmt_for(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::forgeneric_t>();
    enter_scope();

    parse_expr_assignment(stmt.variables(), stmt.iters(), true);
    walk_nodes(stmt.iters());
    walk_nodes(stmt.body());

    exit_scope();
    return nullptr;
}

symbol_t *walker_t::parse_stmt_call(const ast::node_ptr_t &node)
{
    const auto &stmt = node->cast_to<ast::callstmt_t>();
    if (stmt.expression()->type == ast::node_type_t::expr_call) {
        return parse_expr_call(stmt.expression());
    } else {
        return parse_expr_call_1p(stmt.expression());
    }
}

symbol_t *walker_t::parse_expr_member(const ast::node_ptr_t &node)
{
    const auto &expr = node->cast_to<ast::member_t>();
    std::vector<name_t*> bases;
    get_names_from_ident(expr.base(), bases);
    symbol_t *parent = find_parent_by_name(bases);
    if ((parent == nullptr) || !is_table(parent)) {
        return nullptr;
    }

    const auto &ident = expr.identifier()->cast_to<ast::ident_t>();
    auto name = resolve_name(ident.name());
    auto t = parent->cast_to_ptr<table_t*>();
    return t->find_field(name);
}

symbol_t *walker_t::find_parent_by_name(std::vector<name_t*> names)
{
    if (names.empty()) return nullptr;
    auto s = top->get(names[0]->value);
    auto it = names.begin();
    while (s && (++it != names.end())) {
        if (is_table(s)) {
            auto t = s->cast_to_ptr<table_t*>();
            s = t->find_field(*it);
            continue;
        }
        if (is_ref(s)) {
            auto def = s->cast_to_ptr<ref_t*>()->def();
            if (!def || !is_table(def)) {
                break;
            }
            auto t = def->cast_to_ptr<table_t*>();
            s = t->find_field(*it);
            continue;
        }
    }
    if (it == names.end()) {
        return s;
    }
    return nullptr;
}

} // namespace tools

CPPLUA_NS_END

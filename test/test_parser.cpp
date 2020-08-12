#include "cpplua.h"
#include "catch.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "tools/walker.h"

using namespace cpplua;
using namespace cpplua::ast;

TEST_CASE("parse.node.skip_comment") {
    const char *program = "-- hello lua";

    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    auto ast = parser.parse();
    CHECK(ast != nullptr);
    CHECK(ast->is_type(node_type_t::stmt_chunk));
}

TEST_CASE("parse.node.local.var") {
    const char *program = "local x = 1";

    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    auto ast = parser.parse();
    CHECK(ast != nullptr);
    CHECK(ast->is_type(node_type_t::stmt_chunk));
    const auto &chunk = ast->cast_to<chunk_t>();
    CHECK(chunk.body().size() == 1);
    const auto &local_node = chunk.body()[0];
    CHECK(local_node->is_type(node_type_t::stmt_local));
    CHECK(local_node->is_local);
    const auto &local = local_node->cast_to<local_t>();
    CHECK(local.variables().size() == 1);
    CHECK(local.init().size() == 1);
    CHECK(local.variables()[0]->cast_to<ident_t >().name() == "x");
    CHECK(local.init()[0]->cast_to<literal_t >().value_type() == token_type_t::NumericLiteral);
    CHECK(local.init()[0]->cast_to<literal_t >().value() == "1");
}

TEST_CASE("parser.node.local.table") {
    const char *program = "local table = { 1, 2, 3, ['x'] = true, name = 'kitty'}";
    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    auto ast = parser.parse();
    CHECK(ast != nullptr);
    CHECK(ast->is_type(node_type_t::stmt_chunk));
    const auto &chunk = ast->cast_to<chunk_t>();
    CHECK(chunk.body().size() == 1);
    const auto &local = chunk.body()[0]->cast_to<local_t>();
    CHECK(local.is_local);
    CHECK(local.variables().size() == 1);
    CHECK(local.init().size() == 1);
    CHECK(local.variables()[0]->cast_to<ident_t>().name() == "table");
    const auto &expr = local.init()[0]->cast_to<table_construct_expr_t>();
    CHECK(expr.fields().size() == 5);
    for (const auto &field : expr.fields()) {
        if (field->is_type(node_type_t::stmt_table_value)) {
            const auto &val = field->cast_to<tableval_t>();
            CHECK(val.value()->cast_to<literal_t>().value_type() == token_type_t::NumericLiteral);
        } else if (field->is_type(node_type_t::stmt_table_key)){
            const auto &item = field->cast_to<tablekey_t>();
            CHECK(item.key()->cast_to<literal_t>().value_type() == token_type_t::StringLiteral);
            CHECK(item.key()->cast_to<literal_t>().value() == "x");
            CHECK(item.value()->cast_to<literal_t>().value_type() == token_type_t::BooleanLiteral);
            CHECK(item.value()->cast_to<literal_t>().value() == "true");
        } else {
            const auto &item = field->cast_to<tablekey_string_t>();
            CHECK(item.key()->cast_to<ident_t>().name() == "name");
            CHECK(item.value()->cast_to<literal_t >().value_type() == token_type_t::StringLiteral);
            CHECK(item.value()->cast_to<literal_t >().value() == "kitty");
        }
    }
}

TEST_CASE("parse.node.local.function") {
    const char *program = "local function hello(world) end";

    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    auto ast = parser.parse();
    CHECK(ast != nullptr);
    CHECK(ast->is_type(node_type_t::stmt_chunk));
    const auto &chunk = ast->cast_to<chunk_t>();
    CHECK(chunk.body().size() == 1);
    const auto &local_node = chunk.body()[0];
    CHECK(local_node->is_type(node_type_t::stmt_function));
    CHECK(local_node->is_local);
    const auto &local = local_node->cast_to<funcdecl_t>();
    CHECK(local.identifier()->cast_to<ident_t>().name() == "hello");
    CHECK(local.params().size() == 1);
    CHECK(local.body().empty());
}

TEST_CASE("parse.node.local.assignment") {
    const char *program = "local x; x = 1";

    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    auto ast = parser.parse();
    CHECK(ast != nullptr);
    CHECK(ast->is_type(node_type_t::stmt_chunk));
    const auto &chunk = ast->cast_to<chunk_t>();
    CHECK(chunk.body().size() == 2);
    const auto &local_node = chunk.body()[0];
    CHECK(local_node->is_type(node_type_t::stmt_local));
    CHECK(local_node->is_local);
    const auto &local = local_node->cast_to<local_t>();
    const auto &assign = chunk.body()[1];
    CHECK(assign->is_type(node_type_t::stmt_assignment));
    CHECK(assign->cast_to<assignment_t>().vars().size() == 1);
    CHECK(assign->cast_to<assignment_t>().inits().size() == 1);
    const auto &var = assign->cast_to<assignment_t>().vars()[0];
    CHECK(var->cast_to<ident_t>().name() == "x");
}

TEST_CASE("parse.exception.unexpected.eof") {
    const char *program = "local x; x";

    parser_t parser{{"5.1"}, program, (uint32_t)strlen(program)};
    try {
        auto ast = parser.parse();
    } catch (error::syntax_error &e) {
        CHECK(string_t{e.what()} == "[0:9] unexpected identifier 'x' near 'eof'");
    }
}

TEST_CASE("parser.parse.file") {
    const cpplua::string_t path{R"(./test/textures/lexer.lua)"};
    std::ifstream file{path};
    std::stringstream is;
    is << file.rdbuf();
    string_t program = is.str();
    parser_t parser{{"5.1"}, program.c_str(), (uint32_t)program.size()};
    try {
        auto ast = parser.parse();
        std::cout << std::numeric_limits<uint32_t>::max() << std::endl;
        tools::walker_t walker{tools::resolve_file(path), ast};
        walker.go();
        const auto &nn = tools::name_resolver_t::instance();
        std::cout << "names: " << nn.object_count() << std::endl;
        std::cout << "symbols: " << tools::symbol_factory_t::instance().count() << std::endl;
    } catch (error::syntax_error &e) {
        std::cout << e.what() << std::endl;
    }
}

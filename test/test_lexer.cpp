#include "cpplua.h"
#include "catch.hpp"
#include <cstring>
#include <iostream>
#include <set>

CPPLUA_NS::feature_t feature_5_1 = CPPLUA_NS::make_feature("5.1");

TEST_CASE("lexer.comment.single_line") {
    const char* program = "-- hello lua";

    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto comment = lexer.lex();
    CHECK(comment.is_comment());
    CHECK(comment.value() == " hello lua");
}

TEST_CASE("lexer.comment.multi_line") {
    const char* program = R"(--[==[
ssss
xxxxx
xxxx
xxx
]==])";

    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto comment = lexer.lex();
    CHECK(comment.is_comment());
    CHECK(comment.value() == R"(ssss
xxxxx
xxxx
xxx
)");
}

TEST_CASE("lexer.keywords") {
    std::set<CPPLUA_NS::string_t> keywords{
        "do", "if", "in", "or",
        "and", "end", "for", "not",
        "else", "then", "goto",
        "break", "local", "until", "while",
        "elseif", "repeat", "return",
        "function"
    };

    for (auto keyword : keywords) {
        CPPLUA_NS::lexer_t lexer(feature_5_1, keyword.c_str(), keyword.size());
        auto token = lexer.lex();
        CHECK(token.is_keyword());
        CHECK(token.value() == keyword);
    }
}

TEST_CASE("lexer.identifier") {
    const char* program = "foo_0_bar";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto ident = lexer.lex();
    CHECK(ident.is_ident());
    CHECK(ident.value() == "foo_0_bar");
}

TEST_CASE("lexer.punctuator.assignment") {
    const char* program = "=";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto euqal = lexer.lex();
    CHECK(euqal.is_punct());
    CHECK(euqal.value() == "=");
}

TEST_CASE("lexer.punctuator.equal") {
    const char* program = "==";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto euqal = lexer.lex();
    CHECK(euqal.is_punct());
    CHECK(euqal.value() == "==");
}

TEST_CASE("lexer.string.single_line") {
    const char* program = R"("abcded")";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto str = lexer.lex();
    CHECK(str.is_string());
    CHECK(str.value() == "abcded");
}

TEST_CASE("lexer.string.multi_line") {
    const char* program = R"("abc\
ded")";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto str = lexer.lex();
    CHECK(str.is_string());
    CHECK(str.value() == "abc\\\nded");
}

TEST_CASE("lexer.numeric.dec") {
    const char* program = R"(123.123)";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto str = lexer.lex();
    CHECK(str.is_numeric());
    CHECK(str.value() == "123.123");
}

TEST_CASE("lexer.numeric.hex") {
    const char *hexs[] = {
        "0x0.1E",
        "0xA23p-4",
        "0X1.921FB54442D18P+1",
        "0xBEBADA",
        "0xff"
    };
    for (auto hex : hexs) {
        const char* program = hex;
        CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
        auto str = lexer.lex();
        CHECK(str.is_numeric());
        CHECK(str.value() == hex);
    }
}

TEST_CASE("lexer.punctuator.string.cat") {
    const char* program = R"("abc" .. "def")";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    auto abc = lexer.lex();
    CHECK(abc.is_string());
    CHECK(abc.value() == "abc");
    auto cat = lexer.lex();
    CHECK(cat.is_punct());
    CHECK(cat.value() == "..");
    abc = lexer.lex();
    CHECK(abc.is_string());
    CHECK(abc.value() == "def");
}

TEST_CASE("lexer.exception.unfinished_string") {
    const char* program = "\"as3d3dd3";
    CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
    try {
        auto abc = lexer.lex();
    } catch (cpplua::error::syntax_error &e) {
        CHECK(cpplua::string_t{e.what()} == "[0:9] unfinished string near 'as3d3dd3'");
    }
}

// TEST_CASE("lexer.punctuator.less") {
//     const char *program = R"(
// module ("luamodule", package.seeall)

// local _M = {}
// function _M.new()
//     return setmetatable({}, _M)
// end

// local x = 1 + 2
// local y = "123" ... "456"
// )";

//     CPPLUA_NS::lexer_t lexer(feature_5_1, program, strlen(program));
//     while (1) {
//         const auto tok = lexer.lex();
//         std::cout << tok.to_string() << std::endl;
//         if (tok.is_eof()) {
//             break;
//         }
//     }
// }

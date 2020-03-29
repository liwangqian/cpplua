//
// Created by busted on 2020/2/23.
//

#include <iostream>
#include "catch/catch.hpp"
#include "tools/symbol.h"

using namespace cpplua::tools;

TEST_CASE("type define", "[symbol]")
{
    env_t genv{nullptr};

    SECTION("function type")
    {
        func_t *fp = create_symbol<func_t>(resolve_name("foo"), &genv);
        fp->params.push_back(create_symbol<any_t>(resolve_name("p1"), &fp->fenv));
        fp->returns.push_back(create_symbol<number_t>(resolve_name("count"), &fp->fenv));

        auto r1 = create_symbol<boolean_t>(resolve_name("success"), &fp->fenv);
        auto r2 = create_symbol<str_t>(resolve_name("failed_reason"), &fp->fenv);
        auto r3 = create_symbol<func_t>(resolve_name("error"), &fp->fenv);
        fp->returns.push_back(create_symbol<multi_type_t>(r1, r2, r3));
        std::cout << fp->to_string() << std::endl;
    }

    SECTION("type group")
    {
//        type_t *p1, *p2, *p3;
//        function_t *fp;
//        multi_type_t *g1;
//        p1 = create_type<number_t>();
//        p2 = create_type<boolean_t>();
//        p3 = create_type<cpplua::tools::string_t>();
//        fp = create_type<function_t>();
//        g1 = create_type<multi_type_t>(std::initializer_list<type_t*>{p1, p2, p3, fp});
//
//        auto nt = create_type<named_type_t>(resolve_name("var"), g1);
//        std::cout << nt->to_string() << std::endl;
    }

    SECTION("table type")
    {
//        type_t *p1, *p2;
//        table_t *tbl;
//
//        p1 = create_type<number_t>();
//        p2 = create_type<function_t>();
//        tbl = create_type<table_t>();
//
//        tbl->fields.push_back(create_type<named_type_t>(resolve_name("count"), p1));
//        tbl->fields.push_back(create_type<named_type_t>(resolve_name("update"), p2));
//
//        std::cout << tbl->to_string() << std::endl;
    }
}

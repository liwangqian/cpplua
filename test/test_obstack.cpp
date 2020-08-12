//
// Created by busted on 2020/4/11.
//
#include "utils/obstack.h"
#include "catch.hpp"

TEST_CASE("obstack.construction")
{
    cpplua::utils::obstack_t obstack;
    REQUIRE(obstack.alloc<int>(0) != nullptr);
}

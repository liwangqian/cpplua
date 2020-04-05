#include <iostream>
#include "cpplua.h"
#include "catch.hpp"

TEST_CASE("Token is construct correctly") {
    CPPLUA_NS::token_t tok = {CPPLUA_NS::token_type_t::Identifier, "name", {0, 4}};
    CHECK(CPPLUA_NS::token_type_t::Identifier == tok.type());
    CHECK("name" == tok.value());
    CHECK(0 == tok.range().start);
    CHECK(4 == tok.range().end);
    std::cout << tok.to_string() << std::endl;
}

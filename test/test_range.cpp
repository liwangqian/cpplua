#include "range.h"
#include "catch.hpp"

TEST_CASE("Range is construct correctly") {
    CPPLUA_NS::range_t r{};
    CHECK(r.start.line   == 0);
    CHECK(r.start.column == 0);
    CHECK(r.start.offset == 0);

    CHECK(r.end.line   == 0);
    CHECK(r.end.column == 0);
    CHECK(r.end.offset == 0);

    r = {{1, 2, 2}, {2, 0, 4}};
    CHECK(r.start.line   == 1);
    CHECK(r.start.column == 2);
    CHECK(r.start.offset == 2);

    CHECK(r.end.line   == 2);
    CHECK(r.end.column == 0);
    CHECK(r.end.offset == 4);

    r = {{-1u, -1u, -1u}};
    CHECK(r.start.line   == -1u);
    CHECK(r.start.column == -1u);
    CHECK(r.start.offset == -1u);

    CHECK(r.end.line   == 0);
    CHECK(r.end.column == 0);
    CHECK(r.end.offset == 0);
}

TEST_CASE("Range is format correctly") {
    CPPLUA_NS::range_t r = {{0, 1, 2}, {1, 2, 3}};
    CHECK("{ start = { line = 0, column = 1, offset = 2 }, end = { line = 1, column = 2, offset = 3 } }" == r.to_string());
}

//
// Created by busted on 2020/4/5.
//
#include "catch.hpp"
#include "document.h"

TEST_CASE("测试line_info类功能", "[line_info]")
{
    SECTION("case 01")
    {
        cpplua::lineinfo_t line_info;
        for (std::size_t c = 0; c < 20480; ++c) {
            if (c % 10 == 0) line_info.new_line(c);
        }

        REQUIRE(line_info.chunk_count() == 5);
        REQUIRE(line_info.to_position(28) == cpplua::position_t{2, 8, 28});
        REQUIRE(line_info.to_offset(cpplua::position_t{2, 8, 28}) == 28);
    }

    SECTION("case 02")
    {
        cpplua::lineinfo_t line_info;
        for (std::size_t c = 0; c < 9; ++c) {
            if (c % 10 == 0) line_info.new_line(c);
        }

        REQUIRE(line_info.chunk_count() == 1);
        REQUIRE(line_info.to_position(28) == cpplua::position_t{0, 28, 28});
        REQUIRE(line_info.to_offset(cpplua::position_t{0, 28, 28}) == 28);
    }

    SECTION("case 03 - move ctor")
    {
        cpplua::lineinfo_t line_info;
        for (std::size_t c = 0; c < 20480; ++c) {
            if (c % 10 == 0) line_info.new_line(c);
        }

        cpplua::lineinfo_t info(std::move(line_info));

        REQUIRE(info.chunk_count() == 5);
        REQUIRE(info.to_position(28) == cpplua::position_t{2, 8, 28});
        REQUIRE(info.to_offset(cpplua::position_t{2, 8, 28}) == 28);
    }
}

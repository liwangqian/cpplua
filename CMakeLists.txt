cmake_minimum_required(VERSION 3.15)
project(cpplua)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/ast)
include_directories(third_party)
include_directories(third_party/include)
include_directories(third_party/include/catch)
include_directories(third_party/include/nlohmann)
add_subdirectory(third_party/fmt EXCLUDE_FROM_ALL)

add_executable(cpplua
        include/ast/assign.h
        include/ast/base.h
        include/ast/binary.h
        include/ast/break.h
        include/ast/call.h
        include/ast/chunk.h
        include/ast/cond.h
        include/ast/dostmt.h
        include/ast/func.h
        include/ast/goto.h
        include/ast/ident.h
        include/ast/index.h
        include/ast/label.h
        include/ast/literal.h
        include/ast/local.h
        include/ast/loop.h
        include/ast/member.h
        include/ast/repeat.h
        include/ast/return.h
        include/ast/table.h
        include/ast/unary.h
        include/ast/while.h
        include/cpplua.h
        include/ast.h
        include/def.h
        include/error.h
        include/feature.h
        include/keyword.h
        include/lexer.h
        include/marker.h
        include/parser.h
        include/range.h
        include/scope.h
        include/stack.h
        include/string_view.h
        include/token.h
        include/vm.h
        include/vm/endian.h
        include/vm/chunk.h
        include/vm/constant.h
        include/vm/instruction.h
        include/vm/prototype.h
        include/vm/upvalue.h
        include/vm/reader.h
        src/keyword.cpp
        src/lexer.cpp
        src/parser.cpp
        test/test_lexer.cpp
        test/test_main.cpp
        test/test_parser.cpp
        test/test_range.cpp
        test/test_token.cpp
        test/test_vm.cpp
        third_party/include/catch/catch.hpp
        third_party/include/nlohmann/json.hpp)

target_link_libraries(cpplua fmt-header-only)
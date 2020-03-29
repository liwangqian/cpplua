cmake_minimum_required(VERSION 3.15)
project(cpplua)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/ast)
include_directories(include/tools)
include_directories(third_party)
include_directories(third_party/include)
include_directories(third_party/include/catch)
include_directories(third_party/include/nlohmann)
add_subdirectory(third_party/fmt EXCLUDE_FROM_ALL)

add_executable(cpplua
        src/keyword.cpp
        src/lexer.cpp
        src/parser.cpp
        src/tools/walker.cpp
        test/test_lexer.cpp
        test/test_main.cpp
        test/test_parser.cpp
        test/test_range.cpp
        test/test_token.cpp
        test/test_vm.cpp
        test/test_symbol.cpp
        test/test_ast_walker.cpp
        )

target_link_libraries(cpplua fmt-header-only)

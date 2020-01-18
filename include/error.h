#ifndef CPPLUA_ERROR_H
#define CPPLUA_ERROR_H

#include "def.h"
#include "range.h"
#include "fmt/format.h"

CPPLUA_NS_BEGIN

enum parser_error_t {
    Unexpected = 0,
    Expected,
    ExpectedToken,
    UnfinishedString,
    MalformedNumber,
    InvalidVar,
    DecimalEscapeTooLarge,
    InvalidEscape
};

namespace error {

struct syntax_error : public std::exception {
    string_t message;
    std::size_t line;
    std::size_t column;
    std::size_t offset;

    syntax_error(const syntax_error &) = default;

    syntax_error(string_t msg, const position_t &p)
        : message{std::move(msg)}, line{p.line}, column{p.column}, offset{p.offset}
    {
    }

    const char *what() const noexcept override
    {
        return message.c_str();
    }
};

struct unexpected : public syntax_error {
    unexpected(const position_t &p, const char *type, const char *found, const char *near_)
        : syntax_error(fmt::format("[{}:{}] unexpected {} '{}' near '{}'", p.line, p.column, type, found, near_), p)
    {
    }

    unexpected(const position_t &p, const string_t &type, const string_t &found, const string_t &near_)
        : unexpected(p, type.c_str(), found.c_str(), near_.c_str())
    {
    }
};

struct expected : public syntax_error {
    expected(const position_t &p, const char *expect, const char *near_)
        : syntax_error(fmt::format("[{}:{}] '{}' expected near '{}'", p.line, p.column, expect, near_), p)
    {
    }

    expected(const position_t &p, const string_t &expect, const string_t &near_)
        : expected(p, expect.c_str(), near_.c_str())
    {
    }
};

struct unexpected_token : public syntax_error {
    unexpected_token(const position_t &p, const char *type, const char *found)
        : syntax_error(fmt::format("[{}:{}] unexpected {} near '{}'", p.line, p.column, type, found), p)
    {
    }

    unexpected_token(const position_t &p, const string_t &type, const string_t &found)
        : unexpected_token(p, type.c_str(), found.c_str())
    {
    }
};

struct unfinished_string : public syntax_error {
    unfinished_string(const position_t &p, const char *near_)
        : syntax_error(fmt::format("[{}:{}] unfinished string near '{}'", p.line, p.column, near_), p)
    {
    }

    unfinished_string(const position_t &p, const string_t &near_)
        : unfinished_string(p, near_.c_str())
    {
    }
};

struct unfinished_long_string : public syntax_error {
    unfinished_long_string(const position_t &p, const char *near_)
        : syntax_error(fmt::format("[{}:{}] unfinished long string near '{}'", p.line, p.column, near_), p)
    {
    }

    unfinished_long_string(const position_t &p, const string_t &near_)
        : unfinished_long_string(p, near_.c_str())
    {
    }
};

struct unfinished_long_comment : public syntax_error {
    unfinished_long_comment(const position_t &p, const char *near_)
        : syntax_error(fmt::format("[{}:{}] unfinished long comment near '{}'", p.line, p.column, near_), p)
    {
    }

    unfinished_long_comment(const position_t &p, const string_t &near_)
        : unfinished_long_comment(p, near_.c_str())
    {
    }
};

struct malformed_number : public syntax_error {
    malformed_number(const position_t &p, const char *near_)
        : syntax_error(fmt::format("[{}:{}] malformed number near '{}'", p.line, p.column, near_), p)
    {
    }

    malformed_number(const position_t &p, const string_t &near_)
        : malformed_number(p, near_.c_str())
    {
    }
};
} // namespace error
CPPLUA_NS_END
#endif

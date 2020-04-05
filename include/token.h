#ifndef CPPLUA_TOKEN_H
#define CPPLUA_TOKEN_H

#include <string>
#include <memory>
#include "def.h"
#include "keyword.h"
#include "utils/range.h"
#include "fmt/format.h"

CPPLUA_NS_BEGIN

namespace __detail {

enum token_type {
    Invalid = 0,
    EndOfFile = 1,
    Keyword = 2,
    Identifier = 4,
    Punctuator = 8,
    NilLiteral = 16,
    VarargLiteral = 32,
    StringLiteral = 64,
    BooleanLiteral = 128,
    NumericLiteral = 256,
    Comment = 512,
};

} // __detail

// exports
using token_type_t = __detail::token_type;

////////////////////////////////////////////////////////////////

namespace __detail {

class token {
public:
    inline token_type_t type() const
    { return m_type; }

    inline const string_t &value() const
    { return m_value; }

    inline const vrange_t &range() const
    { return m_range; }

    inline keyword_t keyword_id() const
    { return m_keyword; }

    inline bool is_keyword() const
    { return m_type == Keyword; }

    inline bool is_eof() const
    { return m_type == EndOfFile; }

    inline bool is_ident() const
    { return m_type == Identifier; }

    inline bool is_nil() const
    { return m_type == NilLiteral; }

    inline bool is_string() const
    { return m_type == StringLiteral; }

    inline bool is_numeric() const
    { return m_type == NumericLiteral; }

    inline bool is_vararg() const
    { return m_type == VarargLiteral; }

    inline bool is_bool() const
    { return m_type == BooleanLiteral; }

    inline bool is_punct() const
    { return m_type == Punctuator; }

    inline bool is_comment() const
    { return m_type == Comment; };

    inline bool is_literal() const
    {
        return m_type & (StringLiteral |
                         NumericLiteral |
                         BooleanLiteral |
                         NilLiteral |
                         VarargLiteral);
    }

    inline std::string to_string() const
    {
        return fmt::format("{}\n\ttype = {}\n\tvalue = {}\n\trange = {}\n{}",
            '{', m_type, m_value, m_range, '}');
    }

    virtual ~token() = default;
    token() = default;
    token &operator=(const token &) = default;

    inline token(token_type_t type, const string_t& value, vrange_t range)
        : m_type{type}, m_value{value}, m_range{range}
    {
        if (is_keyword()) {
            m_keyword = get_keyword_t(value);
        }
    }

    inline token(token_type_t type, const char *value, vrange_t range)
        : m_type{type}, m_value{value}, m_range{range}
    {
        if (is_keyword()) {
            m_keyword = get_keyword_t(value);
        }
    }

private:
    token_type_t m_type{Invalid};
    string_t m_value;
    vrange_t m_range{0, 0};
    keyword_t m_keyword{NOT_A_KW};
};

} // namespace __detail

// exports
using token_t = __detail::token;

CPPLUA_NS_END

#endif

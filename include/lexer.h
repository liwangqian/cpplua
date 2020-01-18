#ifndef CPPLUA_LEXER_H
#define CPPLUA_LEXER_H

#include "def.h"
#include "token.h"
#include "string_view.h"
#include "feature.h"
#include "error.h"

CPPLUA_NS_BEGIN

namespace __detail {

class lexer {
public:
    lexer(feature_t features, const char *input, std::size_t len);
    lexer() = delete;
    ~lexer() = default;

    token_t lex();

private:
    void skip_space();
    bool skip_eol();

    char get();

    token_t scan_comment();
    token_t scan_ident_or_keyword();
    token_t scan_string_literal();
    token_t scan_long_string_literal();
    token_t scan_numeric_literal();
    token_t scan_vararg_literal();
    token_t scan_punctuator(const char *expr, std::size_t len);

    token_t make_eof() const;

    string_view_t read_hex_literal();
    string_view_t read_dec_literal();
    string_view_t read_long_string(bool is_comment);
    bool expect_n_char(char c, std::size_t n);
    inline position_t curr_position() const;
    void raise(const error::syntax_error &error) const noexcept(false);

    const char *m_input;
    std::size_t m_length;
    std::size_t m_index;
    std::size_t m_line;
    std::size_t m_line_start;
    feature_t m_feature;
};

inline position_t lexer::curr_position() const
{
    return cpplua::position_t{m_line, m_index - m_line_start, m_index};
}

} // namespace __detail

// exports
using lexer_t = __detail::lexer;

CPPLUA_NS_END

#endif

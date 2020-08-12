#ifndef CPPLUA_LEXER_H
#define CPPLUA_LEXER_H

#include "def.h"
#include "token.h"
#include "error.h"
#include "feature.h"
#include "utils/noncopyable.h"
#include "utils/string_view.h"
#include "utils/lineinfo.h"

CPPLUA_NS_BEGIN

namespace __detail {

class lexer : public noncopyable_t {
public:
    ~lexer() = default;
    lexer() = delete;
    lexer(feature_t features, const char *input, uint32_t len);

    token_t lex();
    const utils::lineinfo_t &lineinfo() const;

private:
    void skip_space();
    bool skip_eol();
    void new_line(uint32_t offset);

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
    uint32_t m_length;
    uint32_t m_index;
    feature_t m_feature;
    utils::lineinfo_t m_lineinfo;
};

inline position_t lexer::curr_position() const
{
    return m_lineinfo.to_position(m_index);
}

} // namespace __detail

// exports
using lexer_t = __detail::lexer;

CPPLUA_NS_END

#endif

#include "lexer.h"
#include "keyword.h"
#include <cassert>

CPPLUA_NS_BEGIN

namespace __detail {

inline bool is_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

inline bool is_eol(char c)
{
    return c == '\r' || c == '\n';
}

inline bool is_word_start(char c)
{
    return isalpha(c) || c == '_';
}

inline bool is_word_part(char c)
{
    return isalnum(c) || c == '_';
}

inline bool is_hex_digit(char c)
{
    return isdigit(c) || (c >= 97 && c <= 102) || (c >= 65 && c <= 70);
}

lexer::lexer(feature_t features, const char *input, std::size_t len)
    : m_input{input}, m_length{len}, m_index{0}, m_line{0}, m_line_start{0}, m_feature{features}
{
    new_line(0); // 0是第一行的开始位置
}

token_t lexer::lex()
{
    skip_space();

    const auto c = m_input[m_index];
    const auto p = m_input[m_index + 1];
    if (c == '-' && p == '-') {
        return scan_comment();
    }

    if (m_index >= m_length) {
        return make_eof();
    }

    if (is_word_start(c)) {
        return scan_ident_or_keyword();
    }

    switch (c) {
        case '\'':
        case '\"':
            return scan_string_literal();

        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
            return scan_numeric_literal();

        case '.':
            if (isdigit(p)) return scan_numeric_literal();
            if (p == '.') {
                if (m_input[m_index + 2] == '.') return scan_vararg_literal();
                return scan_punctuator("..", 2);
            }
            return scan_punctuator(".", 1);

        case '=':
            if (p == '=') return scan_punctuator("==", 2);
            return scan_punctuator("=", 1);

        case '>':
            if (m_feature.supports(feature_enum_t::bitwise_operators))
                if (p == '>') return scan_punctuator(">>", 2);
            if (p == '=') return scan_punctuator(">=", 2);
            return scan_punctuator(">", 1);

        case '<':
            if (m_feature.supports(feature_enum_t::bitwise_operators))
                if (p == '<') return scan_punctuator("<<", 2);
            if (p == '=') return scan_punctuator("<=", 2);
            return scan_punctuator("<", 1);

        case '~':
            if (p == '=') return scan_punctuator("~=", 2);
            if (!m_feature.supports(feature_enum_t::bitwise_operators))
                break;
            return scan_punctuator("~", 1);

        case ':':
            if (m_feature.supports(feature_enum_t::lables))
                if (p == ':') return scan_punctuator("::", 2);
            return scan_punctuator(":", 1);

        case '[':
            if (p == '[' || p == '=')
                return scan_long_string_literal();
            return scan_punctuator("[", 1);

        case '/':
            if (m_feature.supports(feature_enum_t::integer_division))
                if (p == '/') return scan_punctuator("//", 2);
            return scan_punctuator("/", 1);

        case '&':
        case '|':
            if (!m_feature.supports(feature_enum_t::bitwise_operators))
                break;

            // fall through
        case '*':
        case '^':
        case '%':
        case ',':
        case '{':
        case '}':
        case ']':
        case '(':
        case ')':
        case ';':
        case '#':
        case '-':
        case '+': {
            const char op[] = {c, '\0'};
            return scan_punctuator(op, 1);
        }

        default:
            break;
    }

    return make_eof();
}

char lexer::get()
{
    auto c = m_input[m_index];
    auto p = m_input[m_index + 1];
    if (is_eol(c)) {
        if (c == '\r' && p == '\n') ++m_index;
        if (c == '\n' && p == '\r') ++m_index;
        ++m_line;
        m_line_start = ++m_index;
        new_line(m_index);
    } else {
        ++m_index;
    }
    return c;
}

void lexer::skip_space()
{
    while (m_index < m_length) {
        if (is_whitespace(m_input[m_index])) {
            ++m_index;
        } else if (!skip_eol()) {
            break;
        }
    }
}

bool lexer::skip_eol()
{
    const auto c = m_input[m_index];
    const auto p = m_input[m_index + 1];
    if (is_eol(c)) {
        if (c == '\r' && p == '\n') ++m_index;
        if (c == '\n' && p == '\r') ++m_index;
        ++m_line;
        m_line_start = ++m_index;
        new_line(m_index);
        return true;
    }
    return false;
}

token_t lexer::scan_comment()
{
    m_index += 2; // --

    string_view_t comment;
    const auto start_index = m_index;
    const auto c = m_input[m_index];

    // 可能是多行注释
    if (c == '[') comment = read_long_string(true);
    if (!comment.is_valid()) { // 单行注释
        while (m_index < m_length) {
            if (is_eol(m_input[m_index])) break;
            ++m_index;
        }
        comment = {m_input + start_index, m_index - start_index};
    }

    return { Comment, comment.to_string(), {start_index, m_index } };
}

token_t lexer::scan_ident_or_keyword()
{
    const auto token_start = m_index;
    while (is_word_part(m_input[++m_index])); // empty

    token_type_t type;
    string_t value{m_input + token_start, m_input + m_index};
    if (is_keyword(value)) {
        type = Keyword;
    } else if (value == "true" || value == "false") {
        type = BooleanLiteral;
    } else if (value == "nil") {
        type = NilLiteral;
    } else {
        type = Identifier;
    }

    return { type, value, { token_start , m_index} };
}

token_t lexer::scan_string_literal()
{
    auto delimiter = m_input[m_index++];
    auto string_start = m_index;
    auto token_start = m_index;

    while (m_index < m_length) {
        const char c = m_input[m_index++];
        if (c == delimiter) break;
        if (c == '\\') {
            get(); // TODO
        }
        if (m_index >= m_length || is_eol(c)) {
            raise(error::unfinished_string{
                {m_line, m_index - m_line_start, m_index},
                {m_input + string_start, m_input + m_index}});
        }
    }

    return {
        StringLiteral,
        string_t(m_input + string_start, m_input + m_index - 1),
        { token_start, m_index }
    };
}

token_t lexer::scan_long_string_literal()
{
    auto start_index = m_index;
    auto value = read_long_string(false);
    assert(value.is_valid());

    return { StringLiteral, value.to_string(), { start_index, m_index } };
}

token_t lexer::scan_numeric_literal()
{
    auto token_start = m_index;
    auto c = m_input[m_index];
    auto p = m_input[m_index + 1];
    auto value = (c == '0' && (p == 'x' || p == 'X')) ?
                 read_hex_literal() : read_dec_literal();
    return { NumericLiteral, value.to_string(), { token_start, m_index } };
}

token_t lexer::scan_vararg_literal()
{
    std::size_t token_start = m_index;
    m_index += 3;
    return { VarargLiteral, "...", { token_start, m_index } };
}

token_t lexer::scan_punctuator(const char *expr, std::size_t len)
{
    auto token_start = m_index;
    m_index += len;
    return { Punctuator, expr, { token_start, m_index} };
}

string_view_t lexer::read_hex_literal()
{
    const auto token_start = m_index;
    m_index += 2;

    if (!is_hex_digit(m_input[m_index])) {
        raise(error::malformed_number{curr_position(), string_t{m_input + token_start, m_input + m_index}});
    }

    while (is_hex_digit(m_input[m_index])) ++m_index;
    if (m_input[m_index] == '.') {
        ++m_index;
        while (is_hex_digit(m_input[m_index])) ++m_index;
    }

    auto c = m_input[m_index];
    if (c == 'p' || c == 'P') {
        ++m_index;
        c = m_input[m_index];
        if (c == '+' || c == '-') ++m_index;
        if (!isdigit(m_input[m_index])) {
            raise(error::malformed_number{curr_position(), string_t{m_input + token_start, m_input + m_index}});
        }
        while (isdigit(m_input[m_index])) ++m_index;
    }
    return {m_input + token_start, m_index - token_start};
}

string_view_t lexer::read_dec_literal()
{
    const auto token_start = m_index;
    while (isdigit(m_input[m_index])) ++m_index;
    if (m_input[m_index] == '.') {
        ++m_index;
        while (isdigit(m_input[m_index])) ++m_index;
    }

    auto c = m_input[m_index];
    if (c == 'e' || c == 'E') {
        ++m_index;
        c = m_input[m_index];
        if (c == '+' || c == '-') ++m_index;
        c = m_input[m_index];
        if (!isdigit(c)) {
            raise(error::malformed_number{curr_position(), string_t{m_input + token_start, m_input + m_index}});
        }
        while (isdigit(m_input[m_index])) ++m_index;
    }
    return {m_input + token_start, m_index - token_start};
}

string_view_t lexer::read_long_string(bool is_comment)
{
    ++m_index; // skip [

    auto equal_count = 0; //[==..=[
    while (m_input[m_index + equal_count] == '=') ++equal_count;
    if (m_input[m_index + equal_count] != '[') {
        return {};
    }

    m_index += equal_count + 1;
    if (is_eol(m_input[m_index])) skip_eol();

    bool terminator_found = false;
    const auto string_start = m_index;
    while (m_index < m_length) {
        while (is_eol(m_input[m_index])) skip_eol();

        const char c = m_input[m_index++];
        if (c == ']') {
            terminator_found = expect_n_char('=', equal_count);
            terminator_found = (terminator_found || (m_input[m_index + equal_count] == ']'));
        }

        if (terminator_found) {
            const auto string_size = m_index - string_start - 1;
            m_index += equal_count + 1;
            return {m_input + string_start, string_size};
        }
    }

    if (is_comment) {
        raise(error::unfinished_long_comment{curr_position(), "<eof>"});
    } else {
        raise(error::unfinished_long_string{curr_position(), "<eof>"});
    }
}

bool lexer::expect_n_char(char c, std::size_t n)
{
    for (std::size_t i = 0; i < n; ++i) {
        if (m_input[m_index + i] != c) return false;
    }
    return true;
}

void lexer::raise(const error::syntax_error &error) const noexcept(false)
{
    throw error;
}

token_t lexer::make_eof() const
{
    return { EndOfFile, "eof", { m_index, m_index } };
}

void lexer::new_line(uint32_t offset)
{
    m_lineinfo.new_line(offset);
}

const lineinfo_t &lexer::lineinfo() const
{
    return m_lineinfo;
}

} // namespace __detail

CPPLUA_NS_END

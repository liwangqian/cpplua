#ifndef CPPLUA_STRING_VIEW_H
#define CPPLUA_STRING_VIEW_H

#include "def.h"

CPPLUA_NS_BEGIN
namespace __detail {

template<typename _CharT = char>
class string_view {
public:
    typedef _CharT value_type;

    string_view() = default;
    ~string_view() = default;

    string_view(const string_view &) = default;
    string_view &operator=(const string_view &) = default;

    string_view(const _CharT *ref, std::size_t size)
        : m_ref{ref}, m_size(size)
    {}

    _CharT operator[](int index)
    {
        return m_ref[index > 0 ? index : (index + m_size)];
    }

    _CharT at(int index) throw(std::out_of_range)
    {
        if (std::abs(index) >= m_size)
            throw std::out_of_range("index out-of-range");

        return this->operator[](index);
    }

    std::basic_string<_CharT> to_string()
    {
        return std::basic_string<_CharT>(m_ref, m_size);
    }

    bool is_valid() const
    {
        return (m_ref != nullptr) && (m_size > 0);
    }

private:
    const _CharT *m_ref = nullptr;
    std::size_t m_size = 0;
};

} // namespace __detail

using string_view_t = __detail::string_view<char>;
using wstring_view_t = __detail::string_view<wchar_t>;

CPPLUA_NS_END

#endif // CPPLUA_STRING_VIEW_H

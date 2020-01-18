//
// Created by busted on 2019/10/2.
//

#ifndef CPPLUA_STACK_H
#define CPPLUA_STACK_H

#include "def.h"
#include <vector>
#include <algorithm>

CPPLUA_NS_BEGIN
namespace __detail {
template<typename _T>
class stack {
public:
    typedef std::vector<_T> container_type;
    typedef typename std::vector<_T>::size_type size_type;
    typedef _T value_type;

    size_type size() const
    {
        return m_stack.size();
    }

    void push(const value_type &elem)
    {
        m_stack.push_back(elem);
    }

    void pop(size_type n_elem = 1)
    {
        size_type new_size = n_elem >= m_stack.size() ? 0 : m_stack.size() - n_elem;
        m_stack.resize(new_size);
    }

    bool contains(const value_type &elem) const
    {
        return std::find(m_stack.crbegin(), m_stack.crend(), elem) != m_stack.crend();
    }

    value_type &find(const value_type &elem)
    {
        return std::find(m_stack.rbegin(), m_stack.rend(), elem);
    }

    const value_type &find(const value_type &elem) const
    {
        return std::find(m_stack.crbegin(), m_stack.crend(), elem);
    }

    stack() = default;
    ~stack() = default;

private:
    std::vector<_T> m_stack;
};

} // namespace __detail

// exports
template<typename _T> using stack_t = __detail::stack<_T>;

CPPLUA_NS_END

#endif // CPPLUA_STACK_H

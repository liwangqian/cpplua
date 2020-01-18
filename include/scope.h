//
// Created by busted on 2019/10/2.
//

#ifndef CPPLUA_SCOPE_H
#define CPPLUA_SCOPE_H

#include "def.h"
#include "stack.h"

CPPLUA_NS_BEGIN
namespace __detail {
template<typename _T>
class scope {
public:
    using value_type = typename stack_t<_T>::value_type;
    using size_type = typename stack_t<_T>::size_type;

    void push(const value_type &elem)
    {
        m_stack.push(elem);
    }

    scope(stack_t<_T> &stack)
        : m_stack{stack}, m_parent_stack_size{stack.size()}
    {}

    ~scope()
    {
        m_stack.pop(m_parent_stack_size);
    }

private:
    stack_t<_T> &m_stack;
    size_type m_parent_stack_size;
};
} // namespace __detail

// exports
template<typename T> using scope_t = __detail::scope<T>;

CPPLUA_NS_END

#endif //CPPLUA_CPPLUA_SCOPE_H

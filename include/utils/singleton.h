//
// Created by busted on 2020/2/23.
//

#ifndef CPPLUA_SINGLETON_H
#define CPPLUA_SINGLETON_H

#include "def.h"

CPPLUA_NS_BEGIN
namespace tools {

template <typename T>
struct singleton_t {
    static T &instance()
    {
        static T inst{};
        return inst;
    }

    void* operator new(size_t size)
    {
        return &instance();
    }

    void* operator new(size_t size, const std::nothrow_t&) noexcept
    {
        return &instance();
    }

    void operator delete(void* p) {}
    void operator delete(void* p, const std::nothrow_t&) noexcept {}

    void* operator new (std::size_t size, void* ptr) noexcept = delete;
    singleton_t(const singleton_t&) = delete;
    singleton_t(singleton_t&&) = delete;
    singleton_t &operator=(const singleton_t&) = delete;
    singleton_t &operator=(singleton_t&&) = delete;

protected:
    singleton_t() = default;
};

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_SINGLETON_H

//
// Created by busted on 2020/4/5.
//

#ifndef CPPLUA_NONCOPYABLE_H
#define CPPLUA_NONCOPYABLE_H

#include "def.h"

CPPLUA_NS_BEGIN

struct noncopyable_t {
    noncopyable_t() = default;
    noncopyable_t(const noncopyable_t&) = delete;
    noncopyable_t(noncopyable_t&&) = delete;
    noncopyable_t &operator=(const noncopyable_t&) = delete;
    noncopyable_t &operator=(noncopyable_t&&) = delete;
};

CPPLUA_NS_END

#endif //CPPLUA_NONCOPYABLE_H

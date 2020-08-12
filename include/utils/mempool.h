//
// Created by busted on 2020/4/6.
//

#ifndef CPPLUA_MEMPOOL_H
#define CPPLUA_MEMPOOL_H

#include <cstddef>
#include "def.h"

CPPLUA_NS_BEGIN
namespace utils {

template <typename T, std::size_t alignment = sizeof(uintptr_t)>
struct aligned_sizeof {
    static constexpr auto value = (sizeof(T) % alignment) ?
        sizeof(T) + (alignment - sizeof(T) % alignment) : sizeof(T);
};

template <std::size_t CHUNK_SIZE = sizeof(uintptr_t)>
struct memory_chunk {
    uint8_t buffer[CHUNK_SIZE];
    memory_chunk *next;
};

template <typename T>
struct memory_block {
    memory_chunk<> *root;
};

} // namespace utils

CPPLUA_NS_END

#endif //CPPLUA_MEMPOOL_H

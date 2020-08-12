//
// Created by busted on 2020/4/6.
//

#ifndef CPPLUA_OBSTACK_H
#define CPPLUA_OBSTACK_H

#include <cstddef>
#include "def.h"
#include "noncopyable.h"

CPPLUA_NS_BEGIN
namespace utils {
namespace __detail {

template <typename T>
void call_dtor(void *p)
{
    static_cast<T*>(p)->~T();
}

using dtor_ptr = void(*)(void*);

template <typename Allocator>
struct memory_block {
    using allocator_type = Allocator;
    using pointer = typename allocator_type::pointer;
    using size_type = typename allocator_type::size_type;
    using value_type = typename allocator_type::value_type;

    ~memory_block()
    {
        if (memory) {
            allocator.deallocate(memory, memory_count);
        }
    }

    memory_block(allocator_type &&a, size_type capacity)
        : allocator{std::forward<allocator_type>(a)}
        , memory{allocator.allocate(capacity)}
        , memory_count{capacity}
    {
    }

    pointer mem() const
    {
        return memory;
    }

    pointer end_of_mem() const
    {
        return memory + memory_count;
    }

    size_type capacity() const
    {
        return memory_count;
    }

private:
    allocator_type allocator;
    pointer memory;
    size_type memory_count;
};

template <typename Allocator>
struct basic_obstack : noncopyable_t {
    using allocator_type = typename memory_block<Allocator>::allocator_type;
    using pointer = typename memory_block<Allocator>::pointer;
    using size_type = typename memory_block<Allocator>::size_type;
    enum { DEFAULT_STACK_SIZE = 64 * 1024 }; // 默认64K大小

    ~basic_obstack()
    {
        dealloc_all();
    }

    explicit basic_obstack(size_type capacity_in_bytes = DEFAULT_STACK_SIZE,
        allocator_type && allocator = allocator_type())
        : memory{std::forward<allocator_type>(allocator),
                 to_alloc_capacity(capacity_in_bytes)}
    {
        top_stack = reinterpret_cast<uint8_t *>(memory.mem());
    }

    template <typename T, typename ...Args>
    T *alloc(Args&& ... args)
    {
        return mem_available<T>() ? push<T>(std::forward<Args>(args)...) : nullptr;
    }

private:
    struct memory_chunk {
        memory_chunk *prev;
        dtor_ptr dtor;
    };

    template <typename T>
    struct alignment {
        enum { value = std::max(std::alignment_of<T>::value,
            std::alignment_of<memory_chunk>::value)
        };
    };

    template <typename T, std::size_t alignment = sizeof(uintptr_t)>
    struct aligned_sizeof {
        enum { value = (sizeof(T) % alignment) ?
            sizeof(T) + (alignment - sizeof(T) % alignment) : sizeof(T)
        };
    };

    static constexpr auto CHUNK_HEAD_SIZE = aligned_sizeof<memory_chunk>::value;

    using allocator_value_type = typename Allocator::value_type;
    static size_type to_alloc_capacity(size_type size_in_byte)
    {
        return size_in_byte / sizeof(allocator_value_type) +
            (size_in_byte % sizeof(allocator_value_type) ? 1 : 0);
    }

    static uint8_t *to_byte_ptr(void *ptr)
    {
        return reinterpret_cast<uint8_t*>(ptr);
    }

    static size_type offset_to_alignment(const uint8_t *p, const size_type align_to) {
        const auto address = reinterpret_cast<size_type>(p);
        return address % align_to ? (align_to - address % align_to) : 0;
    }

    template <typename T>
    bool mem_available()
    {
        auto padding = offset_to_alignment(top_stack, alignment<T>::value);
        auto mem_require = padding + sizeof(T) + CHUNK_HEAD_SIZE;
        return (top_stack + mem_require) < to_byte_ptr(memory.end_of_mem());
    }

    template <typename T, typename ...Args>
    T *construct_at(uint8_t *mem, Args&& ... args)
    {
        return new(mem) T{std::forward<Args>(args)...};
    }

    template <typename T>
    void allocate()
    {
        top_stack += offset_to_alignment(top_stack, alignment<T>::value);
        auto chunk = reinterpret_cast<memory_chunk*>(top_stack);
        chunk->prev = top_chunk;
        chunk->dtor = &call_dtor<T>;
        top_chunk = chunk;
        top_stack += CHUNK_HEAD_SIZE + sizeof(T);
    }

    uint8_t *top_object()
    {
        return reinterpret_cast<uint8_t *>(top_chunk) + CHUNK_HEAD_SIZE;
    }

    template<typename T, typename... Args>
    T *push(Args &&... args)
    {
        allocate<T>();
        return construct_at<T>(top_object(), std::forward<Args>(args)...);
    }

    void dealloc_all()
    {

    }

    memory_chunk *top_chunk{nullptr};
    uint8_t *top_stack{nullptr};
    memory_block<Allocator> memory;
};

} // namespace __detail

template <typename Allocator>
using basic_obstack_t = __detail::basic_obstack<Allocator>;

using obstack_t = __detail::basic_obstack<std::allocator<std::max_align_t>>;

} // namespace utils

CPPLUA_NS_END

#endif //CPPLUA_OBSTACK_H

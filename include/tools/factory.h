//
// Created by busted on 2020/3/7.
//

#ifndef CPPLUA_FACTORY_H
#define CPPLUA_FACTORY_H

#include "utils/singleton.h"
#include <unordered_map>
#include <forward_list>
#include <algorithm>

CPPLUA_NS_BEGIN
namespace tools {

template <typename T>
struct factory_t : singleton_t<factory_t<T>> {
    template <typename D, typename ...Args>
    auto create(Args ...args)
        -> std::enable_if_t<std::is_base_of<T, D>::value, D*>
    {
        // TODO: 优化内存管理
        auto obj = new D{args...};
        if (std::is_base_of<singleton_t<D>, D>::value) {
            return obj;
        } else {
            objects.push_front(obj);
            ++object_count;
        }
        return obj;
    }

    void reset()
    {
        for (auto obj : objects) delete obj;
        objects.clear();
        object_count = 0;
    }

    void destroy(const T* obj)
    {
        objects.remove(obj);
        --object_count;
        delete obj;
    }

    template <typename Cmp>
    T *find_if(const Cmp &cmp)
    {
        return std::find_if(objects.begin(), objects.end(), cmp);
    }

    auto count() const
    {
        return object_count;
    }

    ~factory_t()
    {
        reset();
    }

private:
    std::forward_list<T*> objects;
    std::size_t object_count;
};

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_FACTORY_H

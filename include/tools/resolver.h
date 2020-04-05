//
// Created by busted on 2020/3/7.
//

#ifndef CPPLUA_RESOLVER_H
#define CPPLUA_RESOLVER_H

#include "utils/singleton.h"
#include <unordered_map>

CPPLUA_NS_BEGIN
namespace tools {

template <typename Key, typename Value>
struct resolver_t : singleton_t<resolver_t<Key, Value>> {
    static resolver_t& instance() {
        static resolver_t resolver{};
        return resolver;
    }

    Value *resolve(const Key& key)
    {
        const auto iter = repository.find(key);
        if (iter != repository.end()) {
            return iter->second;
        } else {
            auto val = new Value{key}; // TODO: 通过obstack进行分配，提升内存效率
            repository[key] = val;
            return val;
        }
    }

    template <typename DT, typename = std::enable_if_t<std::is_base_of<Value, DT>::value>>
    DT *resolve(const Key& key)
    {
        const auto iter = repository.find(key);
        if (iter != repository.end()) {
            return static_cast<DT*>(iter->second);
        } else {
            auto val = new DT{}; // TODO: 通过obstack进行分配，提升内存效率
            repository[key] = val;
            return val;
        }
    }

    std::size_t object_count() const
    {
        return repository.size();
    }

    ~resolver_t()
    {
        for (const auto &pair : repository) {
            delete pair.second;
        }
    }

private:
    resolver_t() = default;

    std::unordered_map<Key, Value*> repository;
};

} // namespace tools

CPPLUA_NS_END

#endif //CPPLUA_RESOLVER_H

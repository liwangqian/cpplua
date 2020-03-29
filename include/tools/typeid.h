//
// Created by busted on 2020/2/22.
//

#ifndef CPPLUA_TYPEID_H
#define CPPLUA_TYPEID_H

#include <atomic>
#include "def.h"

CPPLUA_NS_BEGIN
namespace tools {

struct type_id_generator {
private:
    static uint32_t next()
    {
        static std::atomic<uint32_t> id_{0};
        return id_++;
    }

public:
    template<typename T>
    static uint32_t get()
    {
        static uint32_t id = next();
        return id;
    }
};

} // namespace tools
CPPLUA_NS_END

#endif //CPPLUA_TYPEID_H

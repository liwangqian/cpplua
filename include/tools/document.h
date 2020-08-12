//
// Created by busted on 2020/4/3.
//

#ifndef CPPLUA_DOCUMENT_H
#define CPPLUA_DOCUMENT_H

#include <forward_list>
#include "def.h"
#include "files.h"
#include "env.h"
#include "symbol.h"
#include "utils/lineinfo.h"

CPPLUA_NS_BEGIN
namespace tools {

struct document_t {
    ~document_t()
    {
        for (auto &env : envs) {
            delete env;
        }
    }

    document_t(const string_t &path, uint64_t modified_time)
        : file{resolve_file(path)}, last_modified_time{modified_time}
    {}

    env_t *new_env(env_t *prev)
    {
        auto env = new env_t{prev};
        envs.push_front(env);
        return env;
    }

    file_t *file{nullptr};
    env_t *top{nullptr};
    func_t *fsym{nullptr};
    std::forward_list<env_t*> envs;
    std::forward_list<symbol_t*> exports;
    utils::lineinfo_t line_info;
    uint64_t last_modified_time{0};
    bool expire{false};
};

} // namespace tools
CPPLUA_NS_END

#endif //CPPLUA_DOCUMENT_H

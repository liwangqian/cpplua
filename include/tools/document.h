//
// Created by busted on 2020/4/3.
//

#ifndef CPPLUA_DOCUMENT_H
#define CPPLUA_DOCUMENT_H

#include "def.h"
#include "files.h"

CPPLUA_NS_BEGIN
namespace tools {

struct document_t {
    file_t *file;
    uint64_t last_modified_time;
};

} // namespace tools
CPPLUA_NS_END

#endif //CPPLUA_DOCUMENT_H

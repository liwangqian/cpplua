//
// Created by busted on 2020/4/5.
//

#ifndef CPPLUA_LINEINFO_H
#define CPPLUA_LINEINFO_H

#include <vector>
#include <array>
#include <limits>
#include "def.h"
#include "range.h"

CPPLUA_NS_BEGIN
namespace utils {

struct lineinfo_t {
    enum { LINE_COUNT_PER_CHUNK = 512 }; // 512 * 4 = 2K Bytes, 小于CPU一级缓存
    using line_info_chunk_t = std::array<uint32_t, LINE_COUNT_PER_CHUNK>;
    using chunk_vec_t = std::vector<line_info_chunk_t *>;

    ~lineinfo_t();
    lineinfo_t();

    lineinfo_t(const lineinfo_t &) = delete;
    lineinfo_t &operator=(const lineinfo_t &) = delete;

    lineinfo_t(lineinfo_t &&rhs) noexcept;
    lineinfo_t &operator=(lineinfo_t &&rhs) noexcept;

    void new_line(uint32_t offset);

    position_t to_position(uint32_t offset) const;
    uint32_t to_offset(position_t pos) const;

    std::size_t chunk_count() const
    {
        return chunks.size();
    }

private:
    chunk_vec_t chunks;
    line_info_chunk_t *current_chunk{nullptr};
    uint32_t slot{0};
};

} // namespace utils
CPPLUA_NS_END

#endif //CPPLUA_LINEINFO_H

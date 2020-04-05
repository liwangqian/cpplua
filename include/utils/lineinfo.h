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

struct lineinfo_t {
    enum { LINE_COUNT_PER_CHUNK = 512 }; // 512 * 4 = 2K Bytes, 小于CPU一级缓存
    using line_info_chunk_t = std::array<uint32_t, LINE_COUNT_PER_CHUNK>;
    using chunk_vec_t = std::vector<line_info_chunk_t*>;

    ~lineinfo_t()
    {
        for (auto chunk : chunks) {
            delete chunk;
        }
    }

    lineinfo_t()
    {
        chunks.reserve(6);
        current_chunk = new line_info_chunk_t{};
        current_chunk->fill(std::numeric_limits<uint32_t>::max());
        chunks.push_back(current_chunk);
    }

    lineinfo_t(const lineinfo_t&) = delete;
    lineinfo_t &operator=(const lineinfo_t&) = delete;

    lineinfo_t(lineinfo_t &&rhs) noexcept
    {
        std::swap(chunks, rhs.chunks);
        std::swap(current_chunk, rhs.current_chunk);
        std::swap(slot, rhs.slot);
    }

    lineinfo_t &operator=(lineinfo_t &&rhs) noexcept
    {
        std::swap(chunks, rhs.chunks);
        std::swap(current_chunk, rhs.current_chunk);
        std::swap(slot, rhs.slot);
        return *this;
    }

    void new_line(uint32_t offset)
    {
        (*current_chunk)[slot++] = offset;
        if (slot >= current_chunk->size()) {
            current_chunk = new line_info_chunk_t{};
            current_chunk->fill(std::numeric_limits<uint32_t>::max());
            chunks.push_back(current_chunk);
            slot = 0;
        }
    }

    position_t to_position(uint32_t offset)
    {
        uint32_t line{0};
        uint32_t column{0};
        for (std::size_t i = 0; i < chunks.size(); ++i) {
            if (chunks[i]->back() < offset) {
                continue;
            }
            auto pos = std::upper_bound(chunks[i]->begin(), chunks[i]->end(), offset) - 1;
            line = i * LINE_COUNT_PER_CHUNK + std::distance(chunks[i]->begin(), pos);
            column = offset - *pos;
            break;
        }
        return position_t{line, column, offset};
    }

    uint32_t to_offset(position_t pos)
    {
        auto chunk_id = pos.line / LINE_COUNT_PER_CHUNK;
        auto slot_id = pos.line % LINE_COUNT_PER_CHUNK;
        if (chunk_id >= chunks.size()) {
            return 0;
        }
        return (*chunks[chunk_id])[slot_id] + pos.column;
    }

    std::size_t chunk_count() const
    {
        return chunks.size();
    }

private:
    chunk_vec_t chunks;
    line_info_chunk_t *current_chunk{nullptr};
    uint32_t slot{0};
};

CPPLUA_NS_END

#endif //CPPLUA_LINEINFO_H

//
// Created by busted on 2020/4/6.
//

#include "utils/lineinfo.h"

CPPLUA_NS_BEGIN
namespace utils {

lineinfo_t::~lineinfo_t()
{
    for (auto chunk : chunks) {
        delete chunk;
    }
}

lineinfo_t::lineinfo_t()
{
    chunks.reserve(6);
    current_chunk = new line_info_chunk_t{};
    current_chunk->fill(std::numeric_limits<uint32_t>::max());
    chunks.push_back(current_chunk);
}

lineinfo_t::lineinfo_t(lineinfo_t &&rhs) noexcept
{
    std::swap(chunks, rhs.chunks);
    std::swap(current_chunk, rhs.current_chunk);
    std::swap(slot, rhs.slot);
}

lineinfo_t &lineinfo_t::operator=(lineinfo_t &&rhs) noexcept
{
    std::swap(chunks, rhs.chunks);
    std::swap(current_chunk, rhs.current_chunk);
    std::swap(slot, rhs.slot);
    return *this;
}

void lineinfo_t::new_line(uint32_t offset)
{
    (*current_chunk)[slot++] = offset;
    if (slot >= current_chunk->size()) {
        current_chunk = new line_info_chunk_t{};
        current_chunk->fill(std::numeric_limits<uint32_t>::max());
        chunks.push_back(current_chunk);
        slot = 0;
    }
}

position_t lineinfo_t::to_position(uint32_t offset) const
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

uint32_t lineinfo_t::to_offset(position_t pos) const
{
    auto chunk_id = pos.line / LINE_COUNT_PER_CHUNK;
    auto slot_id = pos.line % LINE_COUNT_PER_CHUNK;
    if (chunk_id >= chunks.size()) {
        return 0;
    }
    return (*chunks[chunk_id])[slot_id] + pos.column;
}

} // namespace utils
CPPLUA_NS_END
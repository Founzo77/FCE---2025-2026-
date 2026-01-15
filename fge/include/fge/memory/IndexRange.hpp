#pragma once

#include <cstdint>

namespace fge
{
    struct IndexRange
    {
    public:
        uint32_t m_index;
        uint32_t m_count;

        IndexRange() : m_index(0), m_count(0) {}
        IndexRange(const uint32_t index, const uint32_t count) : m_index(index), m_count(count) {}

        IndexRange(const IndexRange&) = default;
        IndexRange(IndexRange&&) = default;

        IndexRange& operator=(const IndexRange&) = default;
        IndexRange& operator=(IndexRange&&) = default;
    };
}
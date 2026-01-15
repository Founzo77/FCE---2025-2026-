#pragma once

#include <vector>
#include <cstdint>

using std::vector;

namespace fge
{
    class IndexAllocator
    {
    public:
        uint64_t m_nbMaxElements;
        uint64_t m_nbElements;
        vector<bool> m_occupancyBuffer;

    public:
        IndexAllocator();
        IndexAllocator(const IndexAllocator& other);
        IndexAllocator(IndexAllocator&& other);

        void initialize(uint64_t nbMaxElements);

        inline uint64_t getNbMaxElements() const noexcept 
            { return m_nbMaxElements; }
        inline uint64_t getNbElements() const noexcept 
            { return m_nbElements; }

        uint64_t alloc();
        void alloc(const uint64_t index);
        void free(const uint64_t index);
    };
}
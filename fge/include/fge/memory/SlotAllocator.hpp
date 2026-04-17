#pragma once

#include <vector>
#include <cstdint>

using std::vector;

namespace fge
{
    class SlotAllocator
    {
    public:
        uint64_t m_nbMaxElements;
        uint64_t m_nbElements;
        vector<uint64_t> m_occupancyBuffer;

    public:
        SlotAllocator();
        SlotAllocator(const SlotAllocator& other);
        SlotAllocator(SlotAllocator&& other);
        ~SlotAllocator();

        void initialize(uint64_t nbMaxElements);

        void reset();

        inline uint64_t getNbMaxElements() const noexcept 
            { return m_nbMaxElements; }
        inline uint64_t getNbElements() const noexcept 
            { return m_nbElements; }

        uint64_t alloc(const uint64_t count);
        void free(const uint64_t index);
    };
}
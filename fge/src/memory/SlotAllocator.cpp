#include <fge/memory/SlotAllocator.hpp>

#include <fge/utility.hpp>

#include <algorithm>

namespace fge
{
    SlotAllocator::SlotAllocator() : m_nbMaxElements(0), m_nbElements(0)
    {}

    SlotAllocator::SlotAllocator(const SlotAllocator& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements),
        m_occupancyBuffer(other.m_occupancyBuffer)
    {}

    SlotAllocator::SlotAllocator(SlotAllocator&& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements),
        m_occupancyBuffer(std::move(other.m_occupancyBuffer))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
    }

    void SlotAllocator::initialize(uint64_t nbMaxElements)
    {
        m_nbMaxElements = nbMaxElements;
        m_nbElements = 0;
        m_occupancyBuffer.assign(nbMaxElements, 0);
    }

    uint64_t SlotAllocator::alloc(const uint64_t count)
    {
        throwIfFailed(count > 0, "SlotAllocator::alloc: count == 0");
        throwIfFailed(m_nbMaxElements > 0, "SlotAllocator::alloc: allocator not initialized");
        throwIfFailed(count <= m_nbMaxElements, "SlotAllocator::alloc: count exceeds capacity");

        uint64_t runLength = 0;
        uint64_t startIndex = UINT64_MAX;

        for (uint64_t i = 0; i < m_nbMaxElements; ++i)
        {
            if (m_occupancyBuffer[i] == 0)
            {
                if (runLength == 0) startIndex = i;
                ++runLength;
                if (runLength == count)
                {
                    for (uint64_t j = 0; j < count; ++j)
                        m_occupancyBuffer[startIndex + j] = UINT64_MAX;

                    m_occupancyBuffer[startIndex] = count;
                    m_nbElements += count;
                    return startIndex;
                }
            }
            else
            {
                runLength = 0;
                startIndex = UINT64_MAX;
            }
        }

        throwIfFailed(true == false, "SlotAllocator::alloc: no contiguous block found");
        return UINT64_MAX;
    }

    void SlotAllocator::free(const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "SlotAllocator::free: allocator not initialized");
        throwIfFailed(index < m_nbMaxElements, "SlotAllocator::free: invalid index");

        uint8_t size = m_occupancyBuffer[index];
        throwIfFailed(size > 0, "SlotAllocator::free: slot not allocated");

        for (uint64_t i = 0; i < size; ++i)
            m_occupancyBuffer[index + i] = 0;

        m_nbElements -= size;
    }
}
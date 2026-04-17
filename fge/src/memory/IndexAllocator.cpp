#include <fge/memory/IndexAllocator.hpp>

#include <fge/utility.hpp>

#include <algorithm>

namespace fge
{
    IndexAllocator::IndexAllocator() : m_nbMaxElements(0), m_nbElements(0)
    {}

    IndexAllocator::IndexAllocator(const IndexAllocator& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements),
        m_occupancyBuffer(other.m_occupancyBuffer)
    {}

    IndexAllocator::IndexAllocator(IndexAllocator&& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements),
        m_occupancyBuffer(std::move(other.m_occupancyBuffer))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
    }

    void IndexAllocator::initialize(uint64_t nbMaxElements)
    {
        m_nbMaxElements = nbMaxElements;
        m_nbElements = 0;
        m_occupancyBuffer.assign(nbMaxElements, false);
    }

    uint64_t IndexAllocator::alloc()
    {
        throwIfFailed(m_nbElements < m_nbMaxElements, "IndexAllocator::alloc: Full");
        
        for (int64_t i = static_cast<int64_t>(m_nbMaxElements) - 1; i >= 0; i--)
        {
            if (!m_occupancyBuffer[i])
            {
                m_occupancyBuffer[i] = true;
                ++m_nbElements;
                return static_cast<uint64_t>(i);
            }
        }

        return 0;
    }

    void IndexAllocator::alloc(const uint64_t index)
    {
        throwIfFailed(m_occupancyBuffer[index] == false, 
            "IndexAllocator::alloc: Index already allocate");
        m_occupancyBuffer[index] = true;
        m_nbElements++;
    }

    void IndexAllocator::free(const uint64_t index)
    {
        throwIfFailed(m_occupancyBuffer[index] == true, 
            "IndexAllocator::free: index not allocated");
        m_occupancyBuffer[index] = false;
        m_nbElements--;
    }

    CompactIndexAllocator::CompactIndexAllocator() : IndexAllocator()
    {

    }

    CompactIndexAllocator::CompactIndexAllocator(const CompactIndexAllocator& other) 
        : IndexAllocator(other)
    {

    }

    CompactIndexAllocator::CompactIndexAllocator(CompactIndexAllocator&& other)
        : IndexAllocator(std::move(other))
    {

    }

    uint64_t CompactIndexAllocator::alloc()
    {
        throwIfFailed(m_nbElements < m_nbMaxElements, "IndexAllocator::alloc: Full");
        
        for (int64_t i = 0; i < m_nbMaxElements; i++)
        {
            if (!m_occupancyBuffer[i])
            {
                m_occupancyBuffer[i] = true;
                ++m_nbElements;
                return static_cast<uint64_t>(i);
            }
        }

        return 0;
    }
}
#include <fge/memory/CompactStackAllocator.hpp>
#include <utility>
#include <cstring>

namespace fge
{
    CompactStackAllocator::~CompactStackAllocator()
    {
        reset();
    }

    CompactStackAllocator::CompactStackAllocator() :
        m_nbMaxElements(0), m_nbElements(0), m_strideSize(0), m_buffer(0)
    {
        
    }

    CompactStackAllocator::CompactStackAllocator(const CompactStackAllocator& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_buffer(other.m_buffer)
    {}

    CompactStackAllocator::CompactStackAllocator(CompactStackAllocator&& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_buffer(std::move(other.m_buffer))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
        other.m_strideSize = 0;
    }

    void CompactStackAllocator::initialize(uint64_t nbMaxElements, uint64_t strideSize)
    {
        throwIfFailed(nbMaxElements > 0, "CompactStackAllocator::initialize: nbMaxElements == 0");
        throwIfFailed(strideSize > 0, "CompactStackAllocator::initialize: strideSize == 0");

        m_nbMaxElements = nbMaxElements;
        m_strideSize = strideSize;
        m_nbElements = 0;

        m_buffer.clear();
        m_buffer.resize(nbMaxElements * strideSize);
    }

    void CompactStackAllocator::reset()
    {
        m_nbMaxElements = 0;
        m_nbElements = 0;
        m_strideSize = 0;
        m_buffer.clear();
        m_buffer.shrink_to_fit();
    }

    uint64_t CompactStackAllocator::alloc()
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::alloc: not initialized");
        throwIfFailed(m_nbElements < m_nbMaxElements, 
            "CompactStackAllocator::alloc: capacity exceeded");

        const uint64_t index = m_nbElements;
        ++m_nbElements;
        return index;
    }

    uint64_t CompactStackAllocator::alloc(vector<IndexRange>& indicesUpdated)
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::alloc: not initialized");
        throwIfFailed(m_nbElements < m_nbMaxElements, 
            "CompactStackAllocator::alloc: capacity exceeded");

        const uint64_t index = m_nbElements;
        ++m_nbElements;

        indicesUpdated.push_back({ (uint32_t)index, 1 });

        return index;
    }

    void CompactStackAllocator::free(const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::free: not initialized");
        throwIfFailed(m_nbElements > 0, "CompactStackAllocator::free: empty");
        throwIfFailed(index < m_nbElements, "CompactStackAllocator::free: invalid index");

        const uint64_t last = m_nbElements - 1;
        if (index != last)
        {
            // swap-remove: déplacer le dernier élément dans le trou
            uint8_t* dst = m_buffer.data() + index * m_strideSize;
            uint8_t* src = m_buffer.data() + last  * m_strideSize;
            std::memcpy(dst, src, static_cast<size_t>(m_strideSize));
        }
        --m_nbElements;
    }

    void CompactStackAllocator::free(const uint64_t index, vector<IndexRange>& indicesUpdated)
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::free: not initialized");
        throwIfFailed(m_nbElements > 0, "CompactStackAllocator::free: empty");
        throwIfFailed(index < m_nbElements, "CompactStackAllocator::free: invalid index");

        const uint64_t last = m_nbElements - 1;
        if (index != last)
        {
            // swap-remove: déplacer le dernier élément dans le trou
            uint8_t* dst = m_buffer.data() + index * m_strideSize;
            uint8_t* src = m_buffer.data() + last  * m_strideSize;
            std::memcpy(dst, src, static_cast<size_t>(m_strideSize));

            indicesUpdated.push_back({ (uint32_t) index, 1 });
        }
        --m_nbElements;
    }

    void* CompactStackAllocator::operator[](const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::operator[]: not initialized");
        throwIfFailed(index < m_nbElements, "CompactStackAllocator::operator[]: invalid index");

        return static_cast<void*>(m_buffer.data() + index * m_strideSize);
    }

    const void* CompactStackAllocator::operator[](const uint64_t index) const
    {
        throwIfFailed(m_nbMaxElements > 0, "CompactStackAllocator::operator[] const: not initialized");
        throwIfFailed(index < m_nbElements, "CompactStackAllocator::operator[] const: invalid index");

        return static_cast<const void*>(m_buffer.data() + index * m_strideSize);
    }
}

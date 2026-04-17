#include <fge/memory/SafeCompactStackAllocator.hpp>
#include <utility>
#include <cstring>

namespace fge
{
    SafeCompactStackAllocator::SafeCompactStackAllocator() :
        m_nbMaxElements(0), m_nbElements(0), m_strideSize(0), m_buffer(0), m_references(0)
    {
        
    }

    SafeCompactStackAllocator::SafeCompactStackAllocator(const SafeCompactStackAllocator& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_buffer(other.m_buffer), m_references(other.m_references)
    {}

    SafeCompactStackAllocator::SafeCompactStackAllocator(SafeCompactStackAllocator&& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_buffer(std::move(other.m_buffer)),
        m_references(std::move(other.m_references))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
        other.m_strideSize = 0;
    }

    SafeCompactStackAllocator::~SafeCompactStackAllocator()
    {
        reset();
    }

    void SafeCompactStackAllocator::initialize(uint64_t nbMaxElements, uint64_t strideSize)
    {
        throwIfFailed(nbMaxElements > 0, "SafeCompactStackAllocator::initialize: nbMaxElements == 0");
        throwIfFailed(strideSize > 0, "SafeCompactStackAllocator::initialize: strideSize == 0");

        m_nbMaxElements = nbMaxElements;
        m_strideSize = strideSize;
        m_nbElements = 0;

        m_buffer.clear();
        m_buffer.resize(nbMaxElements * strideSize);

        m_references.clear();
        m_references.resize(nbMaxElements * strideSize);
    }

    void SafeCompactStackAllocator::reset()
    {
        m_buffer.clear();
        m_buffer.shrink_to_fit();
        m_references.clear();
        m_references.shrink_to_fit();
        m_nbMaxElements = 0;
        m_nbElements = 0;
        m_strideSize = 0;
    }

    uint64_t SafeCompactStackAllocator::alloc()
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::alloc: not initialized");
        throwIfFailed(m_nbElements < m_nbMaxElements, 
            "SafeCompactStackAllocator::alloc: capacity exceeded");

        const uint64_t index = m_nbElements;
        m_references[index] = 1;
        ++m_nbElements;

        return index;
    }

    uint64_t SafeCompactStackAllocator::allocWithoutSharing()
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::alloc: not initialized");
        throwIfFailed(m_nbElements < m_nbMaxElements, 
            "SafeCompactStackAllocator::alloc: capacity exceeded");

        const uint64_t index = m_nbElements;
        m_references[index] = 1;
        ++m_nbElements;

        return index;
    }
    
    void SafeCompactStackAllocator::share(const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::free: not initialized");
        throwIfFailed(m_nbElements > 0, "SafeCompactStackAllocator::free: empty");
        throwIfFailed(index < m_nbElements, "SafeCompactStackAllocator::free: invalid index");

        m_references[index]++;
    }

    bool SafeCompactStackAllocator::free(const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::free: not initialized");
        throwIfFailed(m_nbElements > 0, "SafeCompactStackAllocator::free: empty");
        throwIfFailed(index < m_nbElements, "SafeCompactStackAllocator::free: invalid index");

        if(m_references[index] > 0)
            m_references[index]--;

        if(m_references[index] == 0)
        {
            const uint64_t last = m_nbElements - 1;
            if (index != last)
            {
                // swap-remove: déplacer le dernier élément dans le trou
                uint8_t* dst = m_buffer.data() + index * m_strideSize;
                uint8_t* src = m_buffer.data() + last  * m_strideSize;
                std::memcpy(dst, src, static_cast<size_t>(m_strideSize));

                m_references[index] = m_references[last];
                m_references[last] = 0;
            }
            --m_nbElements;

            return true;
        }
        else return false;
    }

    void* SafeCompactStackAllocator::operator[](const uint64_t index)
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::operator[]: not initialized");
        throwIfFailed(index < m_nbElements, "SafeCompactStackAllocator::operator[]: invalid index");

        return static_cast<void*>(m_buffer.data() + index * m_strideSize);
    }

    const void* SafeCompactStackAllocator::operator[](const uint64_t index) const
    {
        throwIfFailed(m_nbMaxElements > 0, "SafeCompactStackAllocator::operator[] const: not initialized");
        throwIfFailed(index < m_nbElements, "SafeCompactStackAllocator::operator[] const: invalid index");

        return static_cast<const void*>(m_buffer.data() + index * m_strideSize);
    }
}

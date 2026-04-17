#include <fge/memory/PageBasedAllocator.hpp>

namespace fge
{
    PageBasedAllocator::PageBasedAllocator() : 
        m_nbMaxElements(0), m_nbElements(0), m_strideSize(0),
        m_occupancyBitmap(0, false), m_buffer(0, 0)
    {
        
    }

    PageBasedAllocator::PageBasedAllocator(const PageBasedAllocator& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_occupancyBitmap(other.m_occupancyBitmap), 
        m_freePool(other.m_freePool), m_buffer(other.m_buffer)
    {

    }

    PageBasedAllocator::PageBasedAllocator(PageBasedAllocator&& other) : 
        m_nbMaxElements(other.m_nbMaxElements), m_nbElements(other.m_nbElements), 
        m_strideSize(other.m_strideSize), m_occupancyBitmap(std::move(other.m_occupancyBitmap)), 
        m_freePool(std::move(other.m_freePool)), m_buffer(std::move(other.m_buffer))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
        other.m_strideSize = 0;
    }

    PageBasedAllocator::~PageBasedAllocator()
    {
        reset();
    }

    void PageBasedAllocator::initialize(uint64_t nbMaxElements, uint64_t strideSize)
    {
        m_nbMaxElements = nbMaxElements;
        m_nbElements = 0;
        m_strideSize = strideSize;
        m_occupancyBitmap = std::vector<bool>(nbMaxElements, false);
        m_buffer = std::vector<uint8_t>(nbMaxElements * strideSize, 0);

        m_freePool.reserve(nbMaxElements);
        for (uint64_t i = 0; i < nbMaxElements; ++i)
            m_freePool.push_back(nbMaxElements - 1 - i); // stack LIFO
    }

    void PageBasedAllocator::reset()
    {
        m_nbMaxElements = 0;
        m_nbElements = 0;
        m_strideSize = 0;
        m_greatestIndexOccupy = 0;
        m_occupancyBitmap.clear();
        m_occupancyBitmap.shrink_to_fit();
        m_freePool.clear();
        m_freePool.shrink_to_fit();
        m_buffer.clear();
        m_buffer.shrink_to_fit();
    }

    uint64_t PageBasedAllocator::alloc()
    {
        throwIfFailed(m_freePool.empty() == false, "PageBasedAllocator: No more pages free");

        uint64_t index = m_freePool.back();
        m_freePool.pop_back();

        m_occupancyBitmap[index] = true;
        m_nbElements++;

        if (m_nbElements == 1 || index > m_greatestIndexOccupy)
            m_greatestIndexOccupy = index;

        return index;
    }

    void PageBasedAllocator::free(const uint64_t index)
    {
        throwIfFailed(index < m_nbMaxElements, "PageBasedAllocator: Index invalide");
        throwIfFailed(m_occupancyBitmap[index], "PageBasedAllocator: Page already free");

        m_occupancyBitmap[index] = false;
        m_freePool.push_back(index);
        m_nbElements--;

        if (index == m_greatestIndexOccupy)
        {
            if (m_nbElements == 0)
            {
                m_greatestIndexOccupy = 0;
            }
            else
            {
                m_greatestIndexOccupy = 0;
                for (uint64_t i = index; i > 0; i--)
                {
                    if (m_occupancyBitmap[i])
                    {
                        m_greatestIndexOccupy = i;
                        break;
                    }
                }
            }
        }
    }

    void* PageBasedAllocator::operator[](const uint64_t index)
    {
        throwIfFailed(index < m_nbMaxElements, "PageBasedAllocator::operator[]: Index invalide");

        return static_cast<void*>(&m_buffer[index * m_strideSize]);
    }

    const void* PageBasedAllocator::operator[](const uint64_t index) const
    {
        throwIfFailed(index < m_nbMaxElements, "PageBasedAllocator::operator[]: Index invalide");

        return static_cast<const void*>(&m_buffer[index * m_strideSize]);
    }
}
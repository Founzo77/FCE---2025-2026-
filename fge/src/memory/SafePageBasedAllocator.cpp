#include <fge/memory/SafePageBasedAllocator.hpp>

namespace fge
{
    SafePageBasedAllocator::SafePageBasedAllocator() :
        m_nbMaxElements(0),
        m_nbElements(0),
        m_strideSize(0),
        m_greatestIndexOccupy(0),
        m_occupancyBitmap(0, false),
        m_buffer(0, 0),
        m_nbReferenced(0, 0)
    {
        
    }

    SafePageBasedAllocator::SafePageBasedAllocator(const SafePageBasedAllocator& other) :
        m_nbMaxElements(other.m_nbMaxElements),
        m_nbElements(other.m_nbElements),
        m_strideSize(other.m_strideSize),
        m_greatestIndexOccupy(other.m_greatestIndexOccupy),
        m_occupancyBitmap(other.m_occupancyBitmap),
        m_freePool(other.m_freePool),
        m_buffer(other.m_buffer),
        m_nbReferenced(other.m_nbReferenced)
    {
    }

    SafePageBasedAllocator::SafePageBasedAllocator(SafePageBasedAllocator&& other) :
        m_nbMaxElements(other.m_nbMaxElements),
        m_nbElements(other.m_nbElements),
        m_strideSize(other.m_strideSize),
        m_greatestIndexOccupy(other.m_greatestIndexOccupy),
        m_occupancyBitmap(std::move(other.m_occupancyBitmap)),
        m_freePool(std::move(other.m_freePool)),
        m_buffer(std::move(other.m_buffer)),
        m_nbReferenced(std::move(other.m_nbReferenced))
    {
        other.m_nbMaxElements = 0;
        other.m_nbElements = 0;
        other.m_strideSize = 0;
        other.m_greatestIndexOccupy = 0;
    }

    SafePageBasedAllocator::~SafePageBasedAllocator()
    {
        reset();
    }

    void SafePageBasedAllocator::initialize(uint64_t nbMaxElements, uint64_t strideSize)
    {
        throwIfFailed(m_nbMaxElements == 0, 
            "SafePageBasedAllocator: Structure already initialized");
            
        m_nbMaxElements = nbMaxElements;
        m_nbElements = 0;
        m_strideSize = strideSize;
        m_greatestIndexOccupy = 0;
        m_occupancyBitmap = std::vector<bool>(nbMaxElements, false);
        m_buffer = std::vector<uint8_t>(nbMaxElements * strideSize, 0);
        m_nbReferenced = std::vector<uint64_t>(nbMaxElements, 0);

        m_freePool.reserve(nbMaxElements);
        for (uint64_t i = 0; i < nbMaxElements; ++i)
            m_freePool.push_back(nbMaxElements - 1 - i); // LIFO
    }

    void SafePageBasedAllocator::reset()
    {
        m_nbMaxElements = 0;
        m_nbElements = 0;
        m_strideSize = 0;
        m_greatestIndexOccupy = 0;

        m_occupancyBitmap.clear();
        m_freePool.clear();
        m_buffer.clear();
        m_nbReferenced.clear();

        m_occupancyBitmap.shrink_to_fit();
        m_freePool.shrink_to_fit();
        m_buffer.shrink_to_fit();
        m_nbReferenced.shrink_to_fit();
    }

    uint64_t SafePageBasedAllocator::alloc()
    {
        throwIfFailed(m_freePool.empty() == false, 
            "SafePageBasedAllocator: No more pages free");

        uint64_t index = m_freePool.back();
        m_freePool.pop_back();

        m_occupancyBitmap[index] = true;
        m_nbReferenced[index] = 1;
        m_nbElements++;

        if (m_nbElements == 1 || index > m_greatestIndexOccupy)
            m_greatestIndexOccupy = index;

        return index;
    }

    uint64_t SafePageBasedAllocator::allocWithoutSharing()
    {
        throwIfFailed(m_freePool.empty() == false, 
            "SafePageBasedAllocator: No more pages free");

        uint64_t index = m_freePool.back();
        m_freePool.pop_back();

        m_occupancyBitmap[index] = true;
        m_nbElements++;

        if (m_nbElements == 1 || index > m_greatestIndexOccupy)
            m_greatestIndexOccupy = index;

        return index;
    }

    void SafePageBasedAllocator::share(uint64_t index)
    {
        throwIfFailed(index < m_nbMaxElements, "SafePageBasedAllocator::share: Index invalide");
        throwIfFailed(m_occupancyBitmap[index], "SafePageBasedAllocator::share: Page not allocated");

        m_nbReferenced[index]++;
    }

    bool SafePageBasedAllocator::free(uint64_t index)
    {
        throwIfFailed(index < m_nbMaxElements, "SafePageBasedAllocator::free: Index invalide");
        throwIfFailed(m_occupancyBitmap[index], "SafePageBasedAllocator::free: Page already free");
        
        if(m_nbReferenced[index] > 0)
            m_nbReferenced[index]--;

        if (m_nbReferenced[index] == 0)
        {
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

        return m_nbReferenced[index] == 0;
    }

    void* SafePageBasedAllocator::operator[](uint64_t index)
    {
        throwIfFailed(index < m_nbMaxElements, 
            "SafePageBasedAllocator::operator[]: Index invalide");
        throwIfFailed(m_occupancyBitmap[index], 
            "SafePageBasedAllocator::operator[]: Page non allouée");

        return static_cast<void*>(&m_buffer[index * m_strideSize]);
    }

    const void* SafePageBasedAllocator::operator[](uint64_t index) const
    {
        throwIfFailed(index < m_nbMaxElements, 
            "SafePageBasedAllocator::operator[] const: Index invalide");
        throwIfFailed(m_occupancyBitmap[index], 
            "SafePageBasedAllocator::operator[] const: Page non allouée");

        return static_cast<const void*>(&m_buffer[index * m_strideSize]);
    }
}

#pragma once

#include "../utility.hpp"

#include <vector>
#include <cstdint>
#include <iterator>

using std::vector;

namespace fge
{
    class SafePageBasedAllocator
    {
    private:
        uint64_t m_nbMaxElements;
        uint64_t m_nbElements;
        uint64_t m_strideSize;
        uint64_t m_greatestIndexOccupy;
        vector<bool> m_occupancyBitmap;
        vector<uint64_t> m_freePool;
        vector<uint8_t> m_buffer;
        vector<uint64_t> m_nbReferenced;

    public:
        SafePageBasedAllocator();
        SafePageBasedAllocator(const SafePageBasedAllocator& other);
        SafePageBasedAllocator(SafePageBasedAllocator&& other);
        ~SafePageBasedAllocator();

        void initialize(uint64_t nbMaxElements, uint64_t strideSize);

        void reset();

        inline uint8_t* getBuffer()
        {
            throwIfFailed(m_nbMaxElements > 0, "The data buffer is not allocated");

            return m_buffer.data();
        }

        inline uint64_t getBufferTotalSize() const noexcept 
            { return m_nbMaxElements * m_strideSize; }
        inline uint64_t getNbElements() const noexcept 
            { return m_nbElements; }
        inline uint64_t getStrideSize() const noexcept 
            { return m_strideSize; }
        inline uint64_t getGreatesIndexOccupy() const noexcept
            { return m_greatestIndexOccupy; }
        inline uint64_t getTotalSizeOccupy() const noexcept
            { return (m_nbElements == 0) ? 0 : (m_greatestIndexOccupy + 1) * getStrideSize(); }

        uint64_t alloc();
        uint64_t allocWithoutSharing();
        void share(uint64_t index);
        bool free(uint64_t index);

        void* operator[](uint64_t index);
        const void* operator[](uint64_t index) const;

        template<typename TYPE>
        TYPE& get(const uint64_t index)
        {
            throwIfFailed(index < m_nbMaxElements, 
                "SafePageBasedAllocator::operator[]: Index invalide");
            throwIfFailed(m_occupancyBitmap[index], 
                "SafePageBasedAllocator::operator[]: Page non allouée");

            return *reinterpret_cast<TYPE*>(&m_buffer[index * m_strideSize]);
        }
        template<typename TYPE>
        const TYPE& get(const uint64_t index) const
        {
            throwIfFailed(index < m_nbMaxElements, 
                "SafePageBasedAllocator::operator[] const: Index invalide");
            throwIfFailed(m_occupancyBitmap[index], 
                "SafePageBasedAllocator::operator[] const: Page non allouée");

            return *reinterpret_cast<const TYPE*>(&m_buffer[index * m_strideSize]);
        }

        // TO_DO : defragment() / visualisation bitmap

    private:
        template<class T>
        class OccupiedElementIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            OccupiedElementIterator(const vector<bool>* bitmap,
                                    uint64_t start,
                                    uint64_t upper,
                                    uint8_t* base,
                                    uint64_t stride)
                : m_bitmap(bitmap), m_idx(start), m_max(upper), m_base(base), m_stride(stride)
            {
                advance_to_next_allocated();
            }

            reference operator*() const {
                return *reinterpret_cast<T*>(m_base + m_idx * m_stride);
            }

            pointer operator->() const {
                return reinterpret_cast<T*>(m_base + m_idx * m_stride);
            }

            OccupiedElementIterator& operator++() {
                ++m_idx;
                advance_to_next_allocated();
                return *this;
            }

            bool operator==(const OccupiedElementIterator& other) const {
                return m_bitmap == other.m_bitmap &&
                    m_idx == other.m_idx &&
                    m_max == other.m_max &&
                    m_base == other.m_base &&
                    m_stride == other.m_stride;
            }

            bool operator!=(const OccupiedElementIterator& other) const {
                return !(*this == other);
            }

        private:
            const vector<bool>* m_bitmap{nullptr};
            uint64_t m_idx{0};
            uint64_t m_max{0};
            uint8_t* m_base{nullptr};
            uint64_t m_stride{0};

            void advance_to_next_allocated() {
                if (!m_bitmap) {
                    m_idx = m_max;
                    return;
                }

                while (m_idx < m_max && !(*m_bitmap)[m_idx])
                    ++m_idx;
            }
        };
    
    public:
        template<class T>
        struct OccupiedElementRange {
            OccupiedElementIterator<T> b, e;
            auto begin() const { return b; }
            auto end()   const { return e; }
        };

        template<class T>
        OccupiedElementRange<T> occupiedElements() {
            throwIfFailed(m_nbMaxElements > 0, "occupiedElements: not initialized");
            using Iter = OccupiedElementIterator<T>;
            return {
                Iter(&m_occupancyBitmap, 0, m_nbMaxElements, m_buffer.data(), m_strideSize),
                Iter(&m_occupancyBitmap, m_nbMaxElements, m_nbMaxElements, m_buffer.data(), m_strideSize)
            };
        }

        template<class T>
        OccupiedElementRange<const T> occupiedElements() const {
            throwIfFailed(m_nbMaxElements > 0, "occupiedElements: not initialized");
            using Iter = OccupiedElementIterator<const T>;
            return {
                Iter(&m_occupancyBitmap, 0, m_nbMaxElements,
                    const_cast<uint8_t*>(m_buffer.data()), m_strideSize),
                Iter(&m_occupancyBitmap, m_nbMaxElements, m_nbMaxElements,
                    const_cast<uint8_t*>(m_buffer.data()), m_strideSize)
            };
        }

    private:
        template<class T>
        struct IndexedElement {
            uint32_t index;
            T& value;

            T& operator*() const { return value; }
            T* operator->() const { return &value; }
        };

        template<class T>
        class IndexedOccupiedElementIterator {
        public:
            using Elem = IndexedElement<T>;

            using iterator_category = std::forward_iterator_tag;
            using value_type = Elem;
            using reference = Elem;

            IndexedOccupiedElementIterator(const vector<bool>* bitmap,
                                        uint64_t start,
                                        uint64_t upper,
                                        uint8_t* base,
                                        uint64_t stride)
                : m_bitmap(bitmap), m_idx(start), m_max(upper), m_base(base), m_stride(stride)
            {
                advance();
            }

            reference operator*() const {
                T* ptr = reinterpret_cast<T*>(m_base + m_idx * m_stride);
                return { uint32_t(m_idx), *ptr };
            }

            IndexedOccupiedElementIterator& operator++() {
                ++m_idx;
                advance();
                return *this;
            }

            bool operator==(const IndexedOccupiedElementIterator& other) const {
                return m_bitmap == other.m_bitmap &&
                    m_idx == other.m_idx &&
                    m_max == other.m_max &&
                    m_base == other.m_base &&
                    m_stride == other.m_stride;
            }

            bool operator!=(const IndexedOccupiedElementIterator& other) const {
                return !(*this == other);
            }

        private:
            const vector<bool>* m_bitmap{nullptr};
            uint64_t m_idx{0};
            uint64_t m_max{0};
            uint8_t* m_base{nullptr};
            uint64_t m_stride{0};

            void advance() {
                if (!m_bitmap) {
                    m_idx = m_max;
                    return;
                }

                while (m_idx < m_max && !(*m_bitmap)[m_idx])
                    ++m_idx;
            }
        };

    public:
        template<class T>
        struct IndexedOccupiedElementRange {
            IndexedOccupiedElementIterator<T> b, e;
            auto begin() const { return b; }
            auto end()   const { return e; }
        };

        template<class T>
        IndexedOccupiedElementRange<T> indexedOccupiedElements() {
            throwIfFailed(m_nbMaxElements > 0, "indexedOccupiedElements: not initialized");
            using Iter = IndexedOccupiedElementIterator<T>;
            return {
                Iter(&m_occupancyBitmap, 0, m_nbMaxElements, m_buffer.data(), m_strideSize),
                Iter(&m_occupancyBitmap, m_nbMaxElements, m_nbMaxElements, m_buffer.data(), m_strideSize)
            };
        }

        template<class T>
        IndexedOccupiedElementRange<const T> indexedOccupiedElements() const {
            throwIfFailed(m_nbMaxElements > 0, "indexedOccupiedElements: not initialized");
            using Iter = IndexedOccupiedElementIterator<const T>;
            return {
                Iter(&m_occupancyBitmap, 0, m_nbMaxElements,
                    const_cast<uint8_t*>(m_buffer.data()), m_strideSize),
                Iter(&m_occupancyBitmap, m_nbMaxElements, m_nbMaxElements,
                    const_cast<uint8_t*>(m_buffer.data()), m_strideSize)
            };
        }
    };
}

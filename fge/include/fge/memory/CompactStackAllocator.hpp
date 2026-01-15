#pragma once

#include "IndexRange.hpp"

#include "../utility.hpp"

#include <vector>
#include <cstdint>
#include <iterator>

using std::vector;

namespace fge
{
    class CompactStackAllocator
    {
    public:
        uint64_t m_nbMaxElements;
        uint64_t m_nbElements;
        uint64_t m_strideSize;
        vector<uint8_t> m_buffer;

    public:
        CompactStackAllocator();
        CompactStackAllocator(const CompactStackAllocator& other);
        CompactStackAllocator(CompactStackAllocator&& other);

        void initialize(uint64_t nbMaxElements, uint64_t strideSize);

        inline uint8_t* getBuffer()
        {
            throwIfFailed(m_nbMaxElements > 0, "No elements in the buffer");

            return m_buffer.data();
        }

        inline uint64_t getBufferTotalSize() const noexcept 
            { return m_nbMaxElements * m_strideSize; }
        inline uint64_t getNbElements() const noexcept 
            { return m_nbElements; }
        inline uint64_t getNbMaxElements() const noexcept 
            { return m_nbMaxElements; }
        inline uint64_t getStrideSize() const noexcept 
            { return m_strideSize; }
        inline uint64_t getTotalSizeOccupy() const noexcept
            { return m_nbElements * m_strideSize; }

        uint64_t alloc();
        uint64_t alloc(vector<IndexRange>& indicesUpdated);
        void free(const uint64_t index);
        void free(const uint64_t index, vector<IndexRange>& indicesUpdated);

        void* operator[](const uint64_t index);
        const void* operator[](const uint64_t index) const;

        template<typename TYPE>
        TYPE& get(const uint64_t index)
        {
            throwIfFailed(index < m_nbMaxElements, "CompactStackAllocator::get: Index invalide");

            return *reinterpret_cast<TYPE*>(&m_buffer[index * m_strideSize]);
        }
        template<typename TYPE>
        const TYPE& get(const uint64_t index) const
        {
            throwIfFailed(index < m_nbMaxElements, "CompactStackAllocator::get: Index invalide");

            return *reinterpret_cast<const TYPE*>(&m_buffer[index * m_strideSize]);
        }

    private:
        template<class T>
        class OccupiedElementIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            OccupiedElementIterator(uint64_t idx, uint64_t max, uint8_t* base, uint64_t stride) : 
                m_idx(idx), m_max(max), m_base(base), m_stride(stride) {}

            reference operator*() const {
                return *reinterpret_cast<T*>(m_base + m_idx * m_stride);
            }
            pointer operator->() const {
                return reinterpret_cast<T*>(m_base + m_idx * m_stride);
            }

            OccupiedElementIterator& operator++() {
                ++m_idx;
                return *this;
            }

            bool operator==(const OccupiedElementIterator& other) const {
                return m_idx==other.m_idx && m_max==other.m_max &&
                       m_base==other.m_base && m_stride==other.m_stride;
            }
            bool operator!=(const OccupiedElementIterator& other) const { return !(*this==other); }

        private:
            uint64_t m_idx;
            uint64_t m_max;
            uint8_t* m_base;
            uint64_t m_stride;
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
            throwIfFailed(m_nbMaxElements>0, "occupiedElements: not initialized");
            using Iter = OccupiedElementIterator<T>;
            Iter b(/*idx*/0, /*max*/m_nbElements, m_buffer.data(), m_strideSize);
            Iter e(/*idx*/m_nbElements, /*max*/m_nbElements, m_buffer.data(), m_strideSize);
            return { b, e };
        }
    };
}
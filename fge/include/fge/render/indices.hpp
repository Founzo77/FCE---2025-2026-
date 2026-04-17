#pragma once

#include <cstdint>
#include <functional>

namespace fge
{
    struct LogicalIndex
    {
        uint32_t m_index;

        constexpr LogicalIndex() : m_index(UINT32_MAX) {};
        constexpr LogicalIndex(const uint32_t index) noexcept : m_index(index) {};
        ~LogicalIndex() = default;

        LogicalIndex(const LogicalIndex&) = default;
        LogicalIndex(LogicalIndex&&) = default;

        LogicalIndex& operator=(const LogicalIndex&) = default;
        LogicalIndex& operator=(LogicalIndex&&) = default;

        constexpr bool operator==(const LogicalIndex& other) const 
            { return m_index == other.m_index; }
        constexpr bool operator!=(const LogicalIndex& other) const 
            { return m_index != other.m_index; }
    };

    constexpr LogicalIndex NULL_LOGICAL_INDEX = { UINT32_MAX };

    struct PhysicalIndex
    {
        uint32_t m_index;

        constexpr PhysicalIndex() : m_index(0) {};
        constexpr PhysicalIndex(const uint32_t index) noexcept : m_index(index) {};
        ~PhysicalIndex() = default;

        PhysicalIndex(const PhysicalIndex&) = default;
        PhysicalIndex(PhysicalIndex&&) = default;

        PhysicalIndex& operator=(const PhysicalIndex&) = default;
        PhysicalIndex& operator=(PhysicalIndex&&) = default;
        
        constexpr bool operator==(const PhysicalIndex& other) const 
            { return m_index == other.m_index; }
        constexpr bool operator!=(const PhysicalIndex& other) const 
            { return m_index != other.m_index; }
    };

    constexpr PhysicalIndex NULL_PHYSICAL_INDEX = { UINT32_MAX };
}

template<> struct std::hash<fge::LogicalIndex>
{
    size_t operator()(const fge::LogicalIndex& v) const noexcept
    {
        return std::hash<uint32_t>{}(v.m_index);
    }
};

template<> struct std::hash<fge::PhysicalIndex>
{
    size_t operator()(const fge::PhysicalIndex& v) const noexcept
    {
        return std::hash<uint32_t>{}(v.m_index);
    }
};
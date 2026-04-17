#pragma once

#include <cstdint>

namespace fge
{
    struct Background
    {
    public:
        uint32_t m_textureIndex;

        constexpr Background() noexcept : m_textureIndex(UINT32_MAX)
        {}

        constexpr Background(uint32_t textureIndex) noexcept : 
            m_textureIndex(textureIndex)
        {}
        
        Background(const Background&) = default;
        Background(Background&&) = default;

        Background& operator=(const Background&) = default;
        Background& operator=(Background&&) = default;
    };
}
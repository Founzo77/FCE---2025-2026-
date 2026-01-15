#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    struct Light
    {
    public:
        XMFLOAT3 m_radiance;
        uint32_t _padding0;

        XMFLOAT3 m_position;
        uint32_t _padding1;

        Light() = default;

        constexpr Light(XMFLOAT3 radiance, XMFLOAT3 position) noexcept : 
            m_radiance(radiance), m_position(position) {}
        
        Light(const Light&) = default;
        Light(Light&&) = default;

        Light& operator=(const Light&) = default;
        Light& operator=(Light&&) = default;
    };
}
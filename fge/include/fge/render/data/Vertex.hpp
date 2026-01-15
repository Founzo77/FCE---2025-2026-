#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    struct Vertex
    {
    public:
        XMFLOAT3 m_position;
        uint32_t _padding0;
        XMFLOAT3 m_normal;
        uint32_t _padding1;
        XMFLOAT2 m_uv;
        uint32_t _padding2[2];

        Vertex() = default;

        constexpr Vertex(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT2 uv) noexcept : 
            m_position(position), m_normal(normal), m_uv(uv) {}
        
        Vertex(const Vertex&) = default;
        Vertex(Vertex&&) = default;

        Vertex& operator=(const Vertex&) = default;
        Vertex& operator=(Vertex&&) = default;
    };
}
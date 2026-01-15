#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    struct alignas(16) Material
    {
    public:
        XMFLOAT3 m_reflectance;
        uint32_t m_albedoTextureIndex;
        
        XMFLOAT3 m_transmittance;
        uint32_t m_normalTextureIndex;

        XMFLOAT3 m_ks;
        float m_shininess;

        constexpr Material() noexcept : m_reflectance(0, 0, 0), m_albedoTextureIndex(UINT32_MAX), 
            m_transmittance(0, 0, 0), m_normalTextureIndex(UINT32_MAX),
            m_ks({ 0.2, 0.2, 0.2 }), m_shininess(32)
        {}

        constexpr Material(XMFLOAT3 reflectance, XMFLOAT3 transmittance,
            uint32_t albedoTextureIndex, uint32_t normalTextureIndex,
            XMFLOAT3 ks, float shininess) noexcept : 
            m_reflectance(reflectance), m_transmittance(transmittance),
            m_albedoTextureIndex(albedoTextureIndex), m_normalTextureIndex(normalTextureIndex),
            m_ks(ks), m_shininess(shininess)
        {}
        
        Material(const Material&) = default;
        Material(Material&&) = default;

        Material& operator=(const Material&) = default;
        Material& operator=(Material&&) = default;
    };
}
#pragma once

#include "EngineComponent.hpp"

#include <fge/render/indices.hpp>

#include <DirectXMath.h>

using fge::LogicalIndex;

using namespace DirectX;

namespace fce
{
    class LightPointComponent : public EngineComponent
    {
    public:
        LogicalIndex m_lightIndex;
        XMFLOAT3 m_radiance;

    public:
        LightPointComponent() = default;
        LightPointComponent(const LogicalIndex lightIndex, const XMFLOAT3 radiance) : 
            m_lightIndex(lightIndex), m_radiance(radiance) {}
        virtual ~LightPointComponent() = default;

        LightPointComponent(const LightPointComponent&) = default;
        LightPointComponent(LightPointComponent&&) = default;

        LightPointComponent& operator=(const LightPointComponent&) = default;
        LightPointComponent& operator=(LightPointComponent&&) = default;
    };
}
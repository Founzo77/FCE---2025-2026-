#pragma once

#include "data/IntegratorType.hpp"

namespace fge
{
    struct RenderConfig
    {
    public:
        IntegratorType m_integratorType;
        uint32_t m_spp;
        uint32_t m_nbRenderFrames;
        uint32_t m_nbComputeFrames;

    public:
        RenderConfig() = default;
        ~RenderConfig() = default;

        RenderConfig(const RenderConfig&) = default;
        RenderConfig& operator=(const RenderConfig&) = default;
        RenderConfig(RenderConfig&& other) = default;
        RenderConfig& operator=(RenderConfig&& other) = default;
    };
}
#pragma once

#include "EngineComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    class RenderComponent : public EngineComponent
    {
    public:
        LogicalIndex m_meshIndex;
        LogicalIndex m_instanceIndex;

    public:
        RenderComponent() = default;
        RenderComponent(const LogicalIndex meshIndex, const LogicalIndex instanceIndex) : 
            m_meshIndex(meshIndex), m_instanceIndex(instanceIndex) {}
        virtual ~RenderComponent() = default;

        RenderComponent(const RenderComponent&) = default;
        RenderComponent(RenderComponent&&) = default;

        RenderComponent& operator=(const RenderComponent&) = default;
        RenderComponent& operator=(RenderComponent&&) = default;
    };
}
#pragma once

#include "EngineComponent.hpp"

#include <fge/render/indices.hpp>
#include <fge/render/data/GeometryReference.hpp>

using fge::LogicalIndex;
using fge::GeometryReference;

namespace fce
{
    class RenderComponent : public EngineComponent
    {
    public:
        GeometryReference m_geometryReference;
        LogicalIndex m_instanceIndex;

    public:
        RenderComponent() = default;
        RenderComponent(const GeometryReference geometryReference, const LogicalIndex instanceIndex) : 
            m_geometryReference(geometryReference), m_instanceIndex(instanceIndex) {}
        virtual ~RenderComponent() = default;

        RenderComponent(const RenderComponent&) = default;
        RenderComponent(RenderComponent&&) = default;

        RenderComponent& operator=(const RenderComponent&) = default;
        RenderComponent& operator=(RenderComponent&&) = default;

        void refreshRender();
    };
}
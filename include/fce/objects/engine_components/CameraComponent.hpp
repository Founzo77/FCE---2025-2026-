#pragma once

#include "EngineComponent.hpp"

#include <fge/render/data/Transform.hpp>
#include <fge/render/data/Camera.hpp>

using fge::Transform;
using fge::Camera;

namespace fce
{
    class CameraComponent : public EngineComponent
    {
    private:
        float m_physicalWidth;
        float m_physicalHeight;
        float m_depth;

    public:
        // TO_DO : initialiser tous ca / setter
        CameraComponent() = default;
        CameraComponent(const float physicalWidth, const float physicalHeight,
            const float depth) : m_physicalWidth(physicalWidth), m_physicalHeight(physicalHeight),
            m_depth(depth) {}
        virtual ~CameraComponent() = default;

        CameraComponent(const CameraComponent&) = default;
        CameraComponent(CameraComponent&&) = default;

        CameraComponent& operator=(const CameraComponent&) = default;
        CameraComponent& operator=(CameraComponent&&) = default;

        Camera getCamera();
    };
}
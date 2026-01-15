#pragma once

#include "EngineComponent.hpp"

#include <fge/render/data/Transform.hpp>

using fge::Transform;

namespace fce
{
    class TransformComponent : public EngineComponent
    {
    private:
        Transform m_transform;

    public:
        TransformComponent() = default;
        TransformComponent(const Transform& transform) : m_transform(transform) {};
        virtual ~TransformComponent() = default;

        TransformComponent(const TransformComponent&) = default;
        TransformComponent(TransformComponent&&) = default;

        TransformComponent& operator=(const TransformComponent&) = default;
        TransformComponent& operator=(TransformComponent&&) = default;

        const Transform& getTransform() const;
        void setTransform(const Transform& transform);
    };
}
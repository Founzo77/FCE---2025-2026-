#pragma once

#include "ScriptComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    struct PlayerComponent : public ScriptComponent
    {
    private:
        float m_moveSpeed;
        float m_rotateSpeed;
        float m_acceleration;

    public:
        PlayerComponent() = default;
        PlayerComponent(const float moveSpeed, const float rotateSpeed,
            const float acceleration) : m_moveSpeed(moveSpeed), m_rotateSpeed(rotateSpeed), 
            m_acceleration(acceleration) {}
        virtual ~PlayerComponent() = default;

        PlayerComponent(const PlayerComponent&) = default;
        PlayerComponent(PlayerComponent&&) = default;

        PlayerComponent& operator=(const PlayerComponent&) = default;
        PlayerComponent& operator=(PlayerComponent&&) = default;

        void update();
    };
}
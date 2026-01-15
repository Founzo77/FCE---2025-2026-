#pragma once

#include "ScriptComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    struct ZombieComponent : public ScriptComponent
    {
    private:
        LogicalIndex m_targetGameObjectIndex;
        float m_moveSpeed;
        float m_rotateSpeed;
        float m_stopDistance;
    
    public:
        ZombieComponent() = default;
        ZombieComponent(const LogicalIndex targetGameObjectIndex, const float moveSpeed,
            const float rotateSpeed, const float stopDistance) : 
            m_targetGameObjectIndex(targetGameObjectIndex), m_moveSpeed(moveSpeed),
            m_rotateSpeed(rotateSpeed), m_stopDistance(stopDistance) {}
        virtual ~ZombieComponent() = default;

        ZombieComponent(const ZombieComponent&) = default;
        ZombieComponent(ZombieComponent&&) = default;

        ZombieComponent& operator=(const ZombieComponent&) = default;
        ZombieComponent& operator=(ZombieComponent&&) = default;

        void update();
    };
}
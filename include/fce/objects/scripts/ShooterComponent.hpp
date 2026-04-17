#pragma once

#include "ScriptComponent.hpp"
#include "BulletComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    struct ShooterComponent : public ScriptComponent
    {
    private:
        LogicalIndex m_bulletTargetId;
        BulletComponent m_bulletData;

    public:
        ShooterComponent() = default;
        ShooterComponent(const LogicalIndex bulletTargetId, const BulletComponent bulletComponent) : 
            m_bulletTargetId(bulletTargetId), m_bulletData(bulletComponent)
        {}
        virtual ~ShooterComponent() = default;

        ShooterComponent(const ShooterComponent&) = default;
        ShooterComponent(ShooterComponent&&) = default;

        ShooterComponent& operator=(const ShooterComponent&) = default;
        ShooterComponent& operator=(ShooterComponent&&) = default;

        void update();
    };
}
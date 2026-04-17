#pragma once

#include "ScriptComponent.hpp"

namespace fce
{
    struct BulletComponent : public ScriptComponent
    {
    private:
        float m_bulletSpeed;
        double m_lifeTime;
        double m_currentLifeTime = 0;

    public:
        BulletComponent() = default;
        BulletComponent(const float bulletSpeed, const double lifeTime) : 
            m_bulletSpeed(bulletSpeed), m_lifeTime(lifeTime)
        {}
        virtual ~BulletComponent() = default;

        BulletComponent(const BulletComponent&) = default;
        BulletComponent(BulletComponent&&) = default;

        BulletComponent& operator=(const BulletComponent&) = default;
        BulletComponent& operator=(BulletComponent&&) = default;

        void update();
    };
}
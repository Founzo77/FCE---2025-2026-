#pragma once

#include "ScriptComponent.hpp"

namespace fce
{
    struct ParticleComponent : public ScriptComponent
    {
    private:
        float m_particleSpeed;
        double m_lifeTime;
        double m_currentLifeTime = 0;

    public:
        ParticleComponent() = default;
        ParticleComponent(const float particleSpeed, const double lifeTime) : 
            m_particleSpeed(particleSpeed), m_lifeTime(lifeTime)
        {}
        virtual ~ParticleComponent() = default;

        ParticleComponent(const ParticleComponent&) = default;
        ParticleComponent(ParticleComponent&&) = default;

        ParticleComponent& operator=(const ParticleComponent&) = default;
        ParticleComponent& operator=(ParticleComponent&&) = default;

        void update();
    };
}
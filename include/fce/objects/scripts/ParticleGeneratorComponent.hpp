#pragma once

#include "ScriptComponent.hpp"
#include "ParticleComponent.hpp"

#include <fge/render/indices.hpp>

using fge::LogicalIndex;

namespace fce
{
    struct ParticleGeneratorComponent : public ScriptComponent
    {
    private:
        LogicalIndex m_particleTargetId;
        ParticleComponent m_particleData;
        double m_generateFrequency;
        double m_currentGenerateFrequency = 0;

    public:
        ParticleGeneratorComponent() = default;
        ParticleGeneratorComponent(const LogicalIndex particleTargetId, const ParticleComponent particleData,
            const double generateFrequency) : 
            m_particleTargetId(particleTargetId), m_particleData(particleData), m_generateFrequency(generateFrequency)
        {}
        virtual ~ParticleGeneratorComponent() = default;

        ParticleGeneratorComponent(const ParticleGeneratorComponent&) = default;
        ParticleGeneratorComponent(ParticleGeneratorComponent&&) = default;

        ParticleGeneratorComponent& operator=(const ParticleGeneratorComponent&) = default;
        ParticleGeneratorComponent& operator=(ParticleGeneratorComponent&&) = default;

        void update();
    };
}
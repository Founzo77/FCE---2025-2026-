#include <fce/objects/scripts/ParticleGeneratorComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>
#include <fce/objects/resources/GameObjectFactory.hpp>

#include <fce/UpdateContext.hpp>

namespace fce
{
    void ParticleGeneratorComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        double dt = ctx.getDeltaTime();

        m_currentGenerateFrequency += dt;

        if (m_currentGenerateFrequency < m_generateFrequency)
            return;

        m_currentGenerateFrequency -= m_generateFrequency;

        TransformComponent* transformComponent = self.getComponent<TransformComponent>();
        if (!transformComponent)
            return;

        const Transform& emitterTransform = transformComponent->getTransform();

        GameObject particle = GameObjectFactory::buildStaticRenderedVolumiqueGameObject(
            emitterTransform, m_particleTargetId);
        particle.addComponent<ParticleComponent>(m_particleData);
        self.getObjectManager()->instantiate(std::move(particle));
    }
}
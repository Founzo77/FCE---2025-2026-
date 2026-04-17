#include <fce/objects/scripts/ParticleComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>
#include <fce/UpdateContext.hpp>

#include <random>

namespace fce
{
    void ParticleComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        double dt = ctx.getDeltaTime();

        m_currentLifeTime += dt;
        if (m_currentLifeTime >= m_lifeTime)
        {
            self.destroy();
            return;
        }

        TransformComponent* transformComponent = self.getComponent<TransformComponent>();
        if (!transformComponent)
            return;

        Transform transform = transformComponent->getTransform();

        // Générateur pseudo-aléatoire (statique, partagé)
        static std::mt19937 rng{ std::random_device{}() };
        static std::uniform_real_distribution<float> noiseDist(-1.0f, 1.0f);

        // Direction principale : vers le haut
        XMFLOAT3 velocity;
        velocity.x = noiseDist(rng) * 0.3f; // dérive latérale
        velocity.y = 1.0f; // montée
        velocity.z = noiseDist(rng) * 0.3f; // dérive latérale

        // Normalisation légère (évite accélérations excessives)
        XMVECTOR v = XMLoadFloat3(&velocity);
        v = XMVector3Normalize(v);
        v *= m_particleSpeed;

        // Atténuation avec l’âge (fumée qui se calme)
        float lifeRatio = static_cast<float>(m_currentLifeTime / m_lifeTime);
        float damping = 1.0f - lifeRatio; // ralentit avec le temps
        v *= damping;

        // Intégration
        XMVECTOR delta = v * static_cast<float>(dt);

        transform.translate(delta);

        transformComponent->setTransform(transform);
    }
}
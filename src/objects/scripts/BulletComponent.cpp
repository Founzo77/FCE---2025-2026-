#include <fce/objects/scripts/BulletComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/UpdateContext.hpp>

using namespace DirectX;

namespace fce
{
    void BulletComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        double dt = ctx.getDeltaTime();

        // Gestion du lifetime
        m_lifeTime -= dt;
        if (m_lifeTime <= 0.0)
        {
            self.destroy();
            return;
        }

        TransformComponent* transformComponent = self.getComponent<TransformComponent>();
        if (!transformComponent)
            return;

        Transform transfom = transformComponent->getTransform();

        transfom.moveForward(m_bulletSpeed * dt);

        transformComponent->setTransform(transfom);
    }
}
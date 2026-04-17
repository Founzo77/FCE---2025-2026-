#include <fce/objects/scripts/ShooterComponent.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

#include <fce/objects/resources/GameObjectFactory.hpp>

#include <fce/UpdateContext.hpp>

#include <fge/io/GlobalLogger.hpp>
using fge::globalLogger;

namespace fce
{
    void ShooterComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        const InputManager& input = ctx.getInput();

        // TO_DO Fix le -1
        if (input.getMouseButton(VK_LBUTTON - 1) == KeyState::PRESSED)
        {
            TransformComponent* position = self.getComponent<TransformComponent>();
            GameObject bullet = GameObjectFactory::buildStaticRenderedGameObject(
                position->getTransform(), m_bulletTargetId);
            bullet.addComponent<BulletComponent>(m_bulletData);

            self.getObjectManager()->instantiate(std::move(bullet));
        }
    }
}
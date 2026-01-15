#include <fce/objects/scripts/DebugPositionComponent.hpp>

#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

#include <fce/UpdateContext.hpp>

#include <fge/io/GlobalLogger.hpp>

using fge::globalLogger;

namespace fce
{
    void DebugPositionComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        const InputManager& input = ctx.getInput();

        TransformComponent* playerTransform = self.getComponent<TransformComponent>();

        if (input.getKeyState('P') == KeyState::PRESSED)
        {
            globalLogger().debug() << playerTransform->getTransform().m_position.x << " " <<
                playerTransform->getTransform().m_position.y << " "
                << playerTransform->getTransform().m_position.z;
        }
    }
}
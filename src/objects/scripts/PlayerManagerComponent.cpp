#include <fce/objects/scripts/PlayerManagerComponent.hpp>

#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

#include <fce/UpdateContext.hpp>

#include <fge/io/GlobalLogger.hpp>
using fge::globalLogger;

namespace fce
{
    void PlayerManagerComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        const InputManager& input = ctx.getInput();
        double dt = ctx.getDeltaTime();

        if (input.getKeyState(VK_RIGHT) == KeyState::PRESSED)
        {
            m_currentId++;
            if(m_currentId >= m_players.size())
            {
                m_currentId = 0;
            }
        }

        if (input.getKeyState(VK_LEFT) == KeyState::PRESSED)
        {
            m_currentId--;
            if(m_currentId < 0)
            {
                m_currentId = m_players.size() - 1;
            }
        }

        GameObject& target = self.getObjectManager()->getGameObject(m_players[m_currentId]);
        TransformComponent* playerTransform = target.getComponent<TransformComponent>();

        float moveSpeed = m_moveSpeed * dt;
        float rotateSpeed = XM_PI / m_rotateSpeed * dt;

        if (input.getKeyState(VK_SHIFT) == KeyState::PRESSED || 
            input.getKeyState(VK_SHIFT) == KeyState::PUSH)
        {
            moveSpeed *= m_acceleration;
        }

        bool hasMoved = false;
        Transform newTransform = playerTransform->getTransform();

        if (input.getKeyState('Z') == KeyState::PRESSED || 
            input.getKeyState('Z') == KeyState::PUSH)
        {
            newTransform.moveForward(moveSpeed);
            hasMoved = true;
        }
        if (input.getKeyState('S') == KeyState::PRESSED || 
            input.getKeyState('S') == KeyState::PUSH)
        {
            newTransform.moveForward(-moveSpeed);
            hasMoved = true;
        }
        if (input.getKeyState('Q') == KeyState::PRESSED || 
            input.getKeyState('Q') == KeyState::PUSH)
        {
            newTransform.moveRight(-moveSpeed);
            hasMoved = true;
        }
        if (input.getKeyState('D') == KeyState::PRESSED || 
            input.getKeyState('D') == KeyState::PUSH)
        {
            newTransform.moveRight(moveSpeed);
            hasMoved = true;
        }
        if (input.getKeyState('A') == KeyState::PRESSED || 
            input.getKeyState('A') == KeyState::PUSH)
        {
            newTransform.m_basis.rotateAroundAxis(newTransform.up(), -rotateSpeed);
            hasMoved = true;
        }
        if (input.getKeyState('E') == KeyState::PRESSED || 
            input.getKeyState('E') == KeyState::PUSH)
        {
            newTransform.m_basis.rotateAroundAxis(newTransform.up(), rotateSpeed);
            hasMoved = true;
        }
        if (input.getKeyState(VK_SPACE) == KeyState::PRESSED || 
            input.getKeyState(VK_SPACE) == KeyState::PUSH)
        {
            newTransform.moveUp(moveSpeed);
            hasMoved = true;
        }
        if (input.getKeyState(VK_CONTROL) == KeyState::PRESSED || 
            input.getKeyState(VK_CONTROL) == KeyState::PUSH)
        {
            newTransform.moveUp(-moveSpeed);
            hasMoved = true;
        }

        POINT delta = input.getDeltaMouse();
        if(delta.x != 0 || delta.y != 0)
        {
            float sensitivity = 0.002f;

            float yaw = delta.x * sensitivity; // gauche/droite
            float pitch = delta.y * sensitivity; // haut/bas

            newTransform.m_basis.rotateAroundAxis(newTransform.up(), yaw);

            //XMVECTOR right = newTransform.right();
            //newTransform.m_basis.rotateAroundAxis(right, pitch);

            newTransform.m_basis.orthonormalize();

            hasMoved = true;
        }

        if(hasMoved)
        {
            playerTransform->setTransform(newTransform);
        }
    }
}
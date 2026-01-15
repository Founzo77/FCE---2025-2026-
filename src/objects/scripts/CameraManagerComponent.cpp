#include <fce/objects/scripts/CameraManagerComponent.hpp>

#include <fce/objects/engine_components/CameraComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

#include <fce/UpdateContext.hpp>

#include <fce/io/InputManager.hpp>

namespace fce
{
    void CameraManagerComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        const InputManager& input = ctx.getInput();

        bool hasCameraChanged = false;

        if (input.getKeyState(VK_RIGHT) == KeyState::PRESSED)
        {
            m_currentId++;
            hasCameraChanged = true;
        }

        if (input.getKeyState(VK_LEFT) == KeyState::PRESSED)
        {
            m_currentId--;
            hasCameraChanged = true;
        }

        if(hasCameraChanged)
        {
            if(m_currentId < 0)
            {
                m_currentId = m_cameras.size() - 1;
            }
            if(m_currentId >= m_cameras.size())
            {
                m_currentId = 0;
            }

            CameraComponent* newMainCamera = 
                m_gameObject->getObjectManager()->getGameObject(m_cameras[m_currentId]).
                getComponent<CameraComponent>();
            m_gameObject->getObjectManager()->setRenderCamera(*newMainCamera);
        }
    }
}
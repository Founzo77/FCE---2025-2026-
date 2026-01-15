#include <fce/objects/engine_components/CameraComponent.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>

namespace fce
{
    Camera CameraComponent::getCamera()
    {
        Camera camera;
        camera.m_position = 
            getGameObject().getComponent<TransformComponent>()->getTransform();
        camera.m_depth = m_depth;
        camera.m_physicalHeight = m_physicalHeight;
        camera.m_physicalWidth = m_physicalWidth;

        return camera;
    }
}
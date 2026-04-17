#include <fce/objects/engine_components/RenderComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

namespace fce
{
    void RenderComponent::refreshRender()
    {
        GameObject& self = getGameObject();
        self.getObjectManager()->onRenderComponentRefreshed(*this);
    }
}
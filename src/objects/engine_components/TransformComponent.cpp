#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

namespace fce
{
    const Transform& TransformComponent::getTransform() const
    {
        return m_transform;
    }

    void TransformComponent::setTransform(const Transform& transform)
    {
        m_transform = transform;
        m_gameObject->getObjectManager()->onComponentMoved(*this);
    }
}
#include <fce/objects/Component.hpp>

namespace fce
{
    void Component::initialize(GameObject* gameObject)
    {
        m_gameObject = gameObject;
    }

    GameObject& Component::getGameObject()
    {
        return *m_gameObject;
    }

    const GameObject& fce::Component::getGameObject() const
    {
        return *m_gameObject;
    }
}
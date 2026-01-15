#include <fce/objects/GameObject.hpp>

namespace fce
{
    void GameObject::initialize(const UpdateContext* updateContext, 
        ObjectManager* objectManager, LogicalIndex index)
    {
        m_updateContext = updateContext;
        m_objectManager = objectManager;
        m_index = index;

        // TO_DO Changer ca par index plutot que this dans les components
        for (auto& componentsPerType : m_components)
        {
            for (auto& component : componentsPerType.second)
            {
                component->initialize(this);
            }
        }

        for (auto& scriptsPerType : m_scripts)
        {
            for (auto& script : scriptsPerType.second)
            {
                script->initialize(this);
            }
        }
    }

    LogicalIndex fce::GameObject::getIndex() const
    {
        return m_index;
    }

    ObjectManager* GameObject::getObjectManager()
    {
        return m_objectManager;
    }

    const UpdateContext* GameObject::getUpdateContext() const
    {
        return m_updateContext;
    }

    void GameObject::update()
    {
        for (auto& scriptsPerType : m_scripts)
        {
            for (auto& script : scriptsPerType.second)
            {
                script->update();
            }
        }
    }
}
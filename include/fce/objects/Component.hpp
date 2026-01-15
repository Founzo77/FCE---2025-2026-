#pragma once

#include <memory>

using std::unique_ptr;

namespace fce
{
    class GameObject;

    class Component
    {
    protected:
        GameObject* m_gameObject;

    public:
        Component() = default;
        virtual ~Component() = default;

        Component(const Component&) = default;
        Component(Component&&) = default;

        Component& operator=(const Component&) = default;
        Component& operator=(Component&&) = default;

        void initialize(GameObject* gameObject);

        GameObject& getGameObject();
        const GameObject& getGameObject() const;
    };
}
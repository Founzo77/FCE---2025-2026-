#pragma once

#include "engine_components/EngineComponent.hpp"
#include "scripts/ScriptComponent.hpp"

#include <fge/render/indices.hpp>

#include <unordered_map>
#include <vector>
#include <memory>

using std::vector;
using std::unordered_map;
using std::unique_ptr;

namespace fce
{
    class UpdateContext;
    class ObjectManager;

    class GameObject
    {
    private:
        const UpdateContext* m_updateContext;
        ObjectManager* m_objectManager;
        fge::LogicalIndex m_index;
        unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>> m_components;
        unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>> m_scripts;
        
        friend class ObjectManager;

    public:
        GameObject() = default;
        ~GameObject() = default;
        
        GameObject(
            unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>>&& components,
            unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>>&& scripts) :
            m_index(), m_components(std::move(components)), m_scripts(std::move(scripts)) {}
        GameObject(const fge::LogicalIndex index, 
            unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>>&& components,
            unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>>&& scripts) :
            m_index(index), m_components(std::move(components)), m_scripts(std::move(scripts)) {}

        GameObject(const GameObject&) = delete;
        GameObject(GameObject&&) = default;

        GameObject& operator=(const GameObject&) = delete;
        GameObject& operator=(GameObject&&) = default;

        void initialize(const UpdateContext* updateContext, 
            ObjectManager* objectManager, fge::LogicalIndex index);
        
        fge::LogicalIndex getIndex() const;
        ObjectManager* getObjectManager();
        const UpdateContext* getUpdateContext() const;
        
        template<typename TYPE>
        bool hasComponent() const;
        
        template<typename TYPE>
        void addComponent();
        template<typename TYPE>
        void addComponent(const TYPE& component);

        template<typename TYPE>
        void removeComponent();

        template<typename TYPE>
        TYPE* getComponent();
        template<typename TYPE>
        const TYPE* getComponent() const;

        void update();

        void destroy();
    };

    template<typename TYPE>
    bool GameObject::hasComponent() const
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            auto it = m_components.find(type);
            return (it != m_components.end() && !it->second.empty());
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto it = m_scripts.find(type);
            return (it != m_scripts.end() && !it->second.empty());
        }
        else
        {
            static_assert(isEngineComponent<TYPE>() || isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent.");
            return false;
        }
    }

    template<typename TYPE>
    void GameObject::addComponent()
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            if (hasComponent<TYPE>())
                return;

            auto ptr = std::make_unique<TYPE>();
            ptr->initialize(this);

            m_components[type].push_back(std::move(ptr));
            return;
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto ptr = std::make_unique<TYPE>();
            ptr->initialize(this);

            m_scripts[type].push_back(std::move(ptr));
            return;
        }
        else
        {
            static_assert(isEngineComponent<TYPE>() || isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent.");
        }
    }

    template<typename TYPE>
    void GameObject::addComponent(const TYPE& component)
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            if (hasComponent<TYPE>())
                return;

            auto ptr = std::make_unique<TYPE>(component);
            ptr->initialize(this);

            m_components[type].push_back(std::move(ptr));
            return;
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto ptr = std::make_unique<TYPE>(component);
            ptr->initialize(this);

            m_scripts[type].push_back(std::move(ptr));
            return;
        }
        else
        {
            static_assert(isEngineComponent<TYPE>() || isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent.");
        }
    }

    template<typename TYPE>
    void GameObject::removeComponent()
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            auto it = m_components.find(type);
            if (it == m_components.end() || it->second.empty())
                return;

            // Un seul moteur-comp par type → on efface l’unique élément
            it->second.clear();
            return;
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto it = m_scripts.find(type);
            if (it == m_scripts.end() || it->second.empty())
                return;

            // Possibilité de plusieurs scripts par type (Unity-like)
            it->second.clear();
            return;
        }
        else
        {
            static_assert(isEngineComponent<TYPE>() || isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent.");
        }
    }

    template<typename TYPE>
    TYPE* GameObject::getComponent()
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            auto it = m_components.find(type);
            if (it == m_components.end() || it->second.empty())
                return nullptr;

            EngineComponent* base = it->second.front().get();
            return static_cast<TYPE*>(base);
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto it = m_scripts.find(type);
            if (it == m_scripts.end() || it->second.empty())
                return nullptr;

            ScriptComponent* base = it->second.front().get();
            return static_cast<TYPE*>(base);
        }
        else
        {
            static_assert(isEngineComponent<TYPE>() || isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent.");
            return nullptr;
        }
    }
    
    template<typename TYPE>
    const TYPE* GameObject::getComponent() const
    {
        if constexpr(isEngineComponent<TYPE>())
        {
            EngineComponentType type = engineComponentTypeFrom<TYPE>();

            auto it = m_components.find(type);
            if (it == m_components.end() || it->second.empty())
                return nullptr;

            EngineComponent* base = it->second.front().get();
            return static_cast<TYPE*>(base);
        }
        else if constexpr(isScriptComponent<TYPE>())
        {
            ScriptComponentType type = scriptComponentTypeFrom<TYPE>();

            auto it = m_scripts.find(type);
            if (it == m_scripts.end() || it->second.empty())
                return nullptr;

            ScriptComponent* base = it->second.front().get();
            return static_cast<TYPE*>(base);
        }
        else
        {
            static_assert(
                isEngineComponent<TYPE>() ||
                isScriptComponent<TYPE>(),
                "TYPE must derive from EngineComponent or ScriptComponent."
            );
            return nullptr;
        }
    }
}
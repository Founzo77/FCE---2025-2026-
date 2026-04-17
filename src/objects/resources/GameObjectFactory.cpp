#include <fce/objects/resources/GameObjectFactory.hpp>

#include <fce/objects/engine_components/TransformComponent.hpp>
#include <fce/objects/engine_components/RenderComponent.hpp>

namespace fce
{
    GameObject GameObjectFactory::buildStaticRenderedGameObject(
        const Transform& initialPosition, const fge::LogicalIndex meshIndex)
    {
        // TO_DO Faire que l'insertion des composantes soient mieux / plus lisible / plus fluide

        unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>> components;
        unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>> scripts;

        vector<std::unique_ptr<EngineComponent>> listTransform;
        listTransform.push_back(std::make_unique<TransformComponent>(
            TransformComponent(initialPosition)));
        components.insert({ EngineComponentType::TRANSFORM, std::move(listTransform) });

        vector<std::unique_ptr<EngineComponent>> listRender;
        RenderComponent renderComponent;
        // TO_DO Permettre autre chose que les meshes
        renderComponent.m_geometryReference.m_type = fge::GeometryType::TRIANGLES;
        renderComponent.m_geometryReference.m_geometryIndex = meshIndex.m_index;
        listRender.push_back(std::make_unique<RenderComponent>(renderComponent));
        components.insert({ EngineComponentType::RENDER, std::move(listRender) });
        
        return { std::move(components), std::move(scripts) };
    }

    GameObject GameObjectFactory::buildStaticRenderedVolumiqueGameObject(const fge::Transform& initialPosition, 
        const fge::LogicalIndex volumeIndex)
    {
        // TO_DO Faire que l'insertion des composantes soient mieux / plus lisible / plus fluide

        unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>> components;
        unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>> scripts;

        vector<std::unique_ptr<EngineComponent>> listTransform;
        listTransform.push_back(std::make_unique<TransformComponent>(
            TransformComponent(initialPosition)));
        components.insert({ EngineComponentType::TRANSFORM, std::move(listTransform) });

        vector<std::unique_ptr<EngineComponent>> listRender;
        RenderComponent renderComponent;
        // TO_DO Permettre autre chose que les meshes
        renderComponent.m_geometryReference.m_type = fge::GeometryType::AABB;
        renderComponent.m_geometryReference.m_geometryIndex = volumeIndex.m_index;
        listRender.push_back(std::make_unique<RenderComponent>(renderComponent));
        components.insert({ EngineComponentType::RENDER, std::move(listRender) });
        
        return { std::move(components), std::move(scripts) };
    }
}
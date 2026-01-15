#include <fce/objects/ObjectManager.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>
#include <fce/objects/engine_components/RenderComponent.hpp>

#include <fce/io/XmlReader.hpp>

#include <fge/render/data/Instance.hpp>

using fge::Instance;
using fge::Light;

#include <fge/io/GlobalLogger.hpp>
using fge::globalLogger;

namespace fce
{
    void ObjectManager::initialize(const UpdateContext* updateContext, 
        shared_ptr<FgeScene> renderScene)
    {
        m_updateContext = updateContext;
        m_renderScene = renderScene;

        m_gameObjectPool.initialize();
        m_gameObjectLogicalIndexPool.initialize(m_gameObjectPool.getNbMaxGameObjects());
    }

    void ObjectManager::initialize(const UpdateContext* updateContext, 
        shared_ptr<FgeScene> renderScene, XmlReader& reader)
    {
        initialize(updateContext, renderScene);

        for(size_t i = 0; i < reader.m_gameObjects.size(); i++)
        {
            LogicalIndex index = reader.m_gameObjects[i].getIndex();
            m_gameObjectLogicalIndexPool.alloc(index.m_index);
            m_gameObjectPool.add(std::move(reader.m_gameObjects[i]), index);

            GameObject& newObject = m_gameObjectPool.get(index);
            newObject.initialize(updateContext, this, index);
        }

        m_currentRenderCamera = reader.m_renderCameraGameObject;

        reader.m_gameObjects.clear();
    }

    GameObject& ObjectManager::getGameObject(const LogicalIndex gameObjectIndex)
    {
        return m_gameObjectPool.get(gameObjectIndex);
    }

    void ObjectManager::update()
    {
        updateComponents();
        updateRender();
    }

    void ObjectManager::updateComponents()
    {
        for(auto& gameObject : m_gameObjectPool.iterateOverGameObjects())
        {
            gameObject.update();
        }
    }

    void ObjectManager::updateRender()
    {
        for(LogicalIndex toUpdate : m_renderedGameObjectMoved)
        {
            Instance movedInstance;
            movedInstance.m_transform = m_gameObjectPool.get(toUpdate).
                getComponent<TransformComponent>()->getTransform().worldMatrix();
            RenderComponent* renderComponent = m_gameObjectPool.get(toUpdate).
                getComponent<RenderComponent>();
            movedInstance.m_meshIndex = renderComponent->m_meshIndex.m_index;
            m_renderScene->modifyInstance(movedInstance, renderComponent->m_instanceIndex);
        }

        m_renderedGameObjectMoved.clear();

        for(LogicalIndex LightObjectToUpdate : m_LightGameObjectMoved)
        {
            LightPointComponent* lightPoint = 
                m_gameObjectPool.get(LightObjectToUpdate).getComponent<LightPointComponent>();

            Light movedLight;
            movedLight.m_radiance = lightPoint->m_radiance;
            movedLight.m_position = m_gameObjectPool.get(LightObjectToUpdate).
                getComponent<TransformComponent>()->getTransform().m_position;

            m_renderScene->modifyLight(movedLight, lightPoint->m_lightIndex);
        }

        m_LightGameObjectMoved.clear();

        if(m_hasRenderCameraMoved)
        {
            m_renderScene->setCamera(
                m_gameObjectPool.get(m_currentRenderCamera).
                getComponent<CameraComponent>()->getCamera());
        }
    }

    void ObjectManager::onComponentMoved(const TransformComponent& transform)
    {
        const GameObject& gameObjectMoved = transform.getGameObject();

        if(gameObjectMoved.hasComponent<RenderComponent>())
            m_renderedGameObjectMoved.push_back(gameObjectMoved.getIndex());

        if(gameObjectMoved.hasComponent<LightPointComponent>())
            m_LightGameObjectMoved.push_back(gameObjectMoved.getIndex());

        if(m_currentRenderCamera == gameObjectMoved.getIndex())
            m_hasRenderCameraMoved = true;
    }

    void ObjectManager::setRenderCamera(const CameraComponent& component)
    {
        m_hasRenderCameraMoved = true;
        m_currentRenderCamera = component.getGameObject().getIndex();
    }
}
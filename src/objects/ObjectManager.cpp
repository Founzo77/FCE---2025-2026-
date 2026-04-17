#include <fce/objects/ObjectManager.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>
#include <fce/objects/engine_components/RenderComponent.hpp>

#include <fce/io/server/RemoteCommandQueue.hpp>

#include <fce/io/XmlReader.hpp>

#include <fge/render/data/Instance.hpp>

using fge::Instance;
using fge::Light;
using fge::NULL_LOGICAL_INDEX;

#include <fge/io/GlobalLogger.hpp>
using fge::globalLogger;

namespace fce
{
    void ObjectManager::initialize(const UpdateContext* updateContext, 
        shared_ptr<FgeScene> renderScene, shared_ptr<RemoteCommandQueue> remoteCommandQueue)
    {
        m_updateContext = updateContext;
        m_renderScene = renderScene;
        m_remoteCommandQueue = remoteCommandQueue;

        m_gameObjectPool.initialize();
        m_gameObjectIndexPool.initialize(m_gameObjectPool.getNbMaxGameObjects());
        m_lightIndexPool.initialize(renderScene->getNbMaxLights());
        m_textureIndexPool.initialize(renderScene->getNbMaxTextures());
        m_materialIndexPool.initialize(renderScene->getNbMaxMaterials());
        m_meshIndexPool.initialize(renderScene->getNbMaxMeshes());
        m_instanceIndexPool.initialize(renderScene->getNbMaxInstances());
    }

    void ObjectManager::initialize(const UpdateContext* updateContext, 
        shared_ptr<FgeScene> renderScene, shared_ptr<RemoteCommandQueue> remoteCommandQueue,
        XmlReader& reader)
    {
        initialize(updateContext, renderScene, remoteCommandQueue);

        onInitializeFillGameObject(updateContext, reader);
        onInitializeUpdateSceneIndexPool(reader);
    }
    
    void ObjectManager::onInitializeFillGameObject(const UpdateContext* updateContext, 
        XmlReader& reader)
    {
        for(size_t i = 0; i < reader.m_gameObjects.size(); i++)
        {
            LogicalIndex index = reader.m_gameObjects[i].getIndex();
            m_gameObjectIndexPool.alloc(index.m_index);
            m_gameObjectPool.add(std::move(reader.m_gameObjects[i]));

            GameObject& newObject = m_gameObjectPool.get(index);
            newObject.initialize(updateContext, this, index);
        }

        m_currentRenderCamera = reader.m_renderCameraGameObject;

        reader.m_gameObjects.clear();
    }

    void ObjectManager::onInitializeUpdateSceneIndexPool(XmlReader& reader)
    {
        const fge::SceneDescription& sceneDescription = reader.m_renderReader->m_sceneDescription;

        for(size_t i = 0; i < sceneDescription.m_lights.size(); i++)
            m_lightIndexPool.alloc(i);
        
        for(size_t i = 0; i < sceneDescription.m_materials.size(); i++)
            m_materialIndexPool.alloc(i);

        for(size_t i = 0; i < sceneDescription.m_textures.size(); i++)
            m_textureIndexPool.alloc(i);

        for(const auto& indexMeshMesh : sceneDescription.m_meshes)
            m_meshIndexPool.alloc(indexMeshMesh.first.m_index);

        for(const auto& indexInstanceInstance : sceneDescription.m_instances)
            m_instanceIndexPool.alloc(indexInstanceInstance.first.m_index);
    }

    void ObjectManager::instantiate(GameObject&& gameObject)
    {
        if(gameObject.getIndex() == NULL_LOGICAL_INDEX)
        {
            gameObject.m_index = LogicalIndex(m_gameObjectIndexPool.alloc());
        }

        // TO_DO gerer le mess avec gameObject.m_index
        LogicalIndex logicalIndex = gameObject.m_index;
        m_gameObjectPool.add(std::move(gameObject));
        GameObject& newObject = m_gameObjectPool.get(logicalIndex);
        newObject.initialize(m_updateContext, this, logicalIndex);
        RenderComponent* renderComponent = newObject.getComponent<RenderComponent>();

        if(renderComponent != nullptr)
        {
            fge::Instance instance;
            instance.m_geometryReference = renderComponent->m_geometryReference;
            TransformComponent* transform = newObject.getComponent<TransformComponent>();
            instance.m_transform = transform->getTransform().worldMatrix();

            LogicalIndex instanceIndex = m_instanceIndexPool.alloc();

            m_renderScene->addInstance(instance, instanceIndex);
            
            renderComponent->m_instanceIndex = instanceIndex;
            renderComponent->refreshRender();
        }
    }

    void ObjectManager::destroyGameObject(const LogicalIndex gameObjectIndex)
    {
        GameObject& toDestroy = getGameObject(gameObjectIndex);
        RenderComponent* renderComponent = toDestroy.getComponent<RenderComponent>();

        if(renderComponent != nullptr)
        {
            m_renderScene->deleteInstance(renderComponent->m_instanceIndex);
            m_instanceIndexPool.free(renderComponent->m_instanceIndex.m_index);
        }

        m_gameObjectPool.free(gameObjectIndex);
        m_gameObjectIndexPool.free(gameObjectIndex.m_index);
    }

    GameObject& ObjectManager::getGameObject(const LogicalIndex gameObjectIndex)
    {
        return m_gameObjectPool.get(gameObjectIndex);
    }

    void ObjectManager::update()
    {
        updateComponents();
        updateRemoteRequest();
        updateRender();
    }

    void ObjectManager::updateComponents()
    {
        for(auto& gameObject : m_gameObjectPool.iterateOverGameObjects())
        {
            gameObject.update();
        }
    }

    void fce::ObjectManager::updateRemoteRequest()
    {
        while (auto req = m_remoteCommandQueue->pop())
        {
            if (req->m_type == RemoteRequestType::TELEPORTATION)
            {
                RemoteRequestTeleportation tpData = std::get<RemoteRequestTeleportation>(req->m_data);
                GameObject& currentCamera = m_gameObjectPool.get(m_currentRenderCamera);
                TransformComponent* transformCamera = currentCamera.getComponent<TransformComponent>();
                Transform newTranform = transformCamera->getTransform();
                newTranform.setPosition(tpData.m_position);
                transformCamera->setTransform(newTranform);
            }
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
            movedInstance.m_geometryReference = renderComponent->m_geometryReference;
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

    void ObjectManager::onRenderComponentRefreshed(const RenderComponent& render)
    {
        m_renderedGameObjectMoved.push_back(render.getGameObject().getIndex());
    }

    void ObjectManager::setRenderCamera(const CameraComponent& component)
    {
        m_hasRenderCameraMoved = true;
        m_currentRenderCamera = component.getGameObject().getIndex();
    }
}
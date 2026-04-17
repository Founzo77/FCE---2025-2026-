#pragma once

#include "GameObjectMemoryManager.hpp"

#include <fge/memory/IndexAllocator.hpp>
#include <fge/render/indices.hpp>
#include <fge/render/FgeScene.hpp>

#include <unordered_map>
#include <vector>
#include <memory>

using fge::LogicalIndex;
using fge::IndexAllocator;
using fge::FgeScene;

using std::unordered_map;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

namespace fce
{
    class UpdateContext;

    class TransformComponent;
    class RenderComponent;
    class CameraComponent;

    class XmlReader;

    class RemoteCommandQueue;

    class ObjectManager
    {
    private:
        GameObjectMemoryManager m_gameObjectPool;
        IndexAllocator m_gameObjectIndexPool;
        IndexAllocator m_lightIndexPool;
        IndexAllocator m_textureIndexPool;
        IndexAllocator m_materialIndexPool;
        IndexAllocator m_meshIndexPool;
        IndexAllocator m_instanceIndexPool;
        const UpdateContext* m_updateContext;
        shared_ptr<FgeScene> m_renderScene;
        vector<LogicalIndex> m_renderedGameObjectMoved;
        vector<LogicalIndex> m_LightGameObjectMoved;
        LogicalIndex m_currentRenderCamera;
        bool m_hasRenderCameraMoved;
        shared_ptr<RemoteCommandQueue> m_remoteCommandQueue;

    public:
        void initialize(const UpdateContext* updateContext, shared_ptr<FgeScene> renderScene,
            shared_ptr<RemoteCommandQueue> remoteCommandQueue);
        void initialize(const UpdateContext* updateContext, shared_ptr<FgeScene> renderScene,
            shared_ptr<RemoteCommandQueue> remoteCommandQueue, XmlReader& reader);
        void onInitializeFillGameObject(const UpdateContext* updateContext, XmlReader& reader);
        void onInitializeUpdateSceneIndexPool(XmlReader& reader);

        void instantiate(GameObject&& gameObject);
        void destroyGameObject(const LogicalIndex gameObjectIndex);

        GameObject& getGameObject(const LogicalIndex gameObjectIndex);

        void update();
    private:
        void updateComponents();
        void updateRemoteRequest();
        void updateRender();

    public:
        void onComponentMoved(const TransformComponent& transform);
        void onRenderComponentRefreshed(const RenderComponent& render);
        void setRenderCamera(const CameraComponent& component);
    };
}
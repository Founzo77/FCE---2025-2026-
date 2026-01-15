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
    class CameraComponent;

    class XmlReader;

    class ObjectManager
    {
    private:
        GameObjectMemoryManager m_gameObjectPool;
        IndexAllocator m_gameObjectLogicalIndexPool;
        const UpdateContext* m_updateContext;
        shared_ptr<FgeScene> m_renderScene;
        vector<LogicalIndex> m_renderedGameObjectMoved;
        vector<LogicalIndex> m_LightGameObjectMoved;
        LogicalIndex m_currentRenderCamera;
        bool m_hasRenderCameraMoved;

    public:
        void initialize(const UpdateContext* updateContext, shared_ptr<FgeScene> renderScene);
        void initialize(const UpdateContext* updateContext, shared_ptr<FgeScene> renderScene,
            XmlReader& reader);

        GameObject& getGameObject(const LogicalIndex gameObjectIndex);

        void update();
    private:
        void updateComponents();
        void updateRender();

    public:
        void onComponentMoved(const TransformComponent& transform);
        void setRenderCamera(const CameraComponent& component);
    };
}
#pragma once

#include <fge/memory/PageBasedAllocator.hpp>
#include <fge/render/indices.hpp>

using fge::PageBasedAllocator;
using fge::LogicalIndex;
using fge::PhysicalIndex;

namespace fce
{
    class GameObject;

    class GameObjectMemoryManager
    {
    private:
        PageBasedAllocator m_objectsPool;
        vector<uint32_t> m_logicalIndexToPhysical;

    public:
        void initialize();

        uint64_t getGameObjectPageSize() const;
        uint64_t getNbMaxGameObjects() const;
        uint32_t getNbGameObjects() const;

        GameObject& get(const LogicalIndex gameObjectIndex);
        const GameObject& get(const LogicalIndex gameObjectIndex) const;

        PageBasedAllocator::OccupiedElementRange<GameObject> iterateOverGameObjects();

        PhysicalIndex add(GameObject&& gameObject);
        void free(const LogicalIndex gameObjectIndex);
    };
}
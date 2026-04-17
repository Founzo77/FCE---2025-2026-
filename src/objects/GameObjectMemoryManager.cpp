#include <fce/objects/GameObjectMemoryManager.hpp>

#include <fce/objects/GameObject.hpp>

namespace fce
{
    void GameObjectMemoryManager::initialize()
    {
        m_objectsPool.initialize(getNbMaxGameObjects(), getGameObjectPageSize());
        m_logicalIndexToPhysical.resize(getNbMaxGameObjects(), UINT32_MAX);
    }

    uint64_t GameObjectMemoryManager::getGameObjectPageSize() const
    {
        return sizeof(GameObject);
    }

    uint64_t GameObjectMemoryManager::getNbMaxGameObjects() const
    {
        return 100;
    }

    uint32_t GameObjectMemoryManager::getNbGameObjects() const
    {
        return m_objectsPool.getNbElements();
    }

    GameObject& GameObjectMemoryManager::get(const LogicalIndex gameObjectIndex)
    {
        return m_objectsPool.get<GameObject>(m_logicalIndexToPhysical[gameObjectIndex.m_index]);
    }

    const GameObject& GameObjectMemoryManager::get(const LogicalIndex gameObjectIndex) const
    {
        return m_objectsPool.get<GameObject>(m_logicalIndexToPhysical[gameObjectIndex.m_index]);
    }

    PageBasedAllocator::OccupiedElementRange<GameObject> 
        GameObjectMemoryManager::iterateOverGameObjects()
    {
        return m_objectsPool.occupiedElements<GameObject>();
    }

    PhysicalIndex GameObjectMemoryManager::add(GameObject&& gameObject)
    {
        uint32_t physicalMeshIndex = m_objectsPool.alloc();
        m_logicalIndexToPhysical[gameObject.getIndex().m_index] = physicalMeshIndex;
        new (m_objectsPool[physicalMeshIndex]) GameObject(std::move(gameObject));

        return { physicalMeshIndex };
    }

    void GameObjectMemoryManager::free(const LogicalIndex objectIndex)
    {
        m_objectsPool.free(m_logicalIndexToPhysical[objectIndex.m_index]);
        m_logicalIndexToPhysical[objectIndex.m_index] = UINT32_MAX;
    }
}
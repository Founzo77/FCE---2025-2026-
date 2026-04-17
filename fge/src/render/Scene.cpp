#include <fge/render/Scene.hpp>

#include <fge/render/data/DeviceCamera.hpp>
#include <fge/render/data/DeviceConstantInformations.hpp>
#include <fge/utility.hpp>

#include <fge/io/SceneDescription.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

#include <unordered_map>

using std::vector;

namespace fge
{
    Scene::~Scene()
    {
        reset();
    }

    void Scene::buildScene(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        throwIfFailed(true == false, "Unimplemented");
        // TO_DO Mettre a jour la scene par defaut et la tester
    }

    void Scene::buildScene(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList, SceneDescription& sceneDescription)
    {
        m_memory = std::make_shared<SceneMemoryManager>();
        m_memory->initialize(device, directCommandList, sceneDescription);

        m_camera = sceneDescription.m_camera;

        // TO_DO : Log de la scene
    }

    void Scene::reset()
    {
        if(m_memory)
            m_memory->reset();
        m_memory.reset();
    }

    const shared_ptr<SceneMemoryManager> Scene::getMemory() const
    {
        return m_memory;
    }

    const Light& Scene::getLight(const LogicalIndex lightIndex)
    {
        return m_memory->getLight(lightIndex);
    }

    void Scene::addLight(Light& newLight, const LogicalIndex lightIndex)
    {
        m_memory->addLight(newLight, lightIndex);
    }

    void Scene::modifyLight(Light& modifiedLight, const LogicalIndex lightIndex)
    {
        m_memory->modifyLight(modifiedLight, lightIndex);
    }

    void Scene::deleteLight(const LogicalIndex lightIndex)
    {
        m_memory->deleteLight(lightIndex);
    }

    const Instance& fge::Scene::getInstance(const LogicalIndex instanceIndex)
    {
        return m_memory->getInstance(instanceIndex);
    }

    void Scene::addInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_memory->addInstance(newInstance, instanceIndex);
    }

    void Scene::modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_memory->modifyInstance(newInstance, instanceIndex);
    }

    void Scene::deleteInstance(const LogicalIndex instanceIndex)
    {
        m_memory->deleteInstance(instanceIndex);
    }

    void Scene::moveCameraForward(float distance)
    {
        m_camera.moveForward(distance);
        m_memory->setCamera(m_camera);
    }

    void Scene::moveCameraRight(float distance)
    {
        m_camera.moveRight(distance);
        m_memory->setCamera(m_camera);
    }

    void Scene::moveCameraUp(float distance)
    {
        m_camera.moveUp(distance);
        m_memory->setCamera(m_camera);
    }

    void Scene::rotateCameraY(float distance)
    {
        m_camera.rotateY(distance);
        m_memory->setCamera(m_camera);
    }

    void Scene::rotateCamera(float x, float y)
    {
        m_camera.rotate(x, y);
        m_memory->setCamera(m_camera);
    }

    void Scene::setCamera(const Camera& camera)
    {
        m_camera = camera;
        m_memory->setCamera(m_camera);
    }

    uint32_t Scene::getNbMaxMeshes()
    {
        return m_memory->getNbMaxMeshes();
    }

    uint32_t Scene::getNbMaxSubMeshes()
    {
        return m_memory->getNbMaxSubMeshes();
    }

    uint32_t Scene::getNbMaxInstances()
    {
        return m_memory->getNbMaxInstances();
    }

    uint32_t Scene::getNbMaxMaterials()
    {
        return m_memory->getNbMaxMaterials();
    }

    uint32_t Scene::getNbMaxTextures()
    {
        return m_memory->getNbMaxTextures();
    }

    uint32_t Scene::getNbMaxLights()
    {
        return m_memory->getNbMaxLights();
    }
}

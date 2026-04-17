#pragma once

#include "FgeScene.hpp"

#include "data/Mesh.hpp"
#include "data/Instance.hpp"
#include "data/Camera.hpp"
#include "data/Texture.hpp"
#include "data/Light.hpp"
#include "data/Material.hpp"

#include "indices.hpp"

#include "memory/SceneMemoryManager.hpp"

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

using namespace Microsoft::WRL;
using std::vector;
using std::shared_ptr;
using std::string;

namespace fge
{
    class DeviceMemorySceneManager;
    class SceneDescription;

    class Scene : public FgeScene
    {
    private:
        shared_ptr<SceneMemoryManager> m_memory;
        Camera m_camera;

    public:
        Scene() = default;
        ~Scene();

        void buildScene(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void buildScene(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, 
            SceneDescription& sceneDescription);

        void reset();

        const shared_ptr<SceneMemoryManager> getMemory() const;
        
        const Light& getLight(const LogicalIndex lightIndex) override;
        void addLight(Light& newLight, const LogicalIndex lightIndex) override;
        void modifyLight(Light& modifiedLight, const LogicalIndex lightIndex) override;
        void deleteLight(const LogicalIndex lightIndex) override;
        
        const Instance& getInstance(const LogicalIndex instanceIndex) override;
        void addInstance(Instance& newInstance, const LogicalIndex instanceIndex) override;
        void modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex) override;
        void deleteInstance(const LogicalIndex instanceIndex) override;

        void moveCameraForward(float distance) override;
        void moveCameraRight(float distance) override;
        void moveCameraUp(float distance) override;
        void rotateCameraY(float distance) override;
        void rotateCamera(float x, float y) override;

        void setCamera(const Camera& camera) override;

        uint32_t getNbMaxMeshes() override;
        uint32_t getNbMaxSubMeshes() override;
        uint32_t getNbMaxInstances() override;
        uint32_t getNbMaxMaterials() override;
        uint32_t getNbMaxTextures() override;
        uint32_t getNbMaxLights() override;
    };
}
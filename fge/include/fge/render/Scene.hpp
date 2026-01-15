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

#include <Windows.h>
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
        ~Scene() = default;

        void buildScene(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void buildScene(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, SceneDescription& sceneDescription);

        const shared_ptr<SceneMemoryManager> getMemory() const;
        
        const Light& getLight(const LogicalIndex lightIndex);
        void addLight(Light& newLight, const LogicalIndex lightIndex);
        void modifyLight(Light& modifiedLight, const LogicalIndex lightIndex);
        void deleteLight(const LogicalIndex lightIndex);
        
        const Instance& getInstance(const LogicalIndex instanceIndex);
        void addInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void deleteInstance(const LogicalIndex instanceIndex);

        void moveCameraForward(float distance);
        void moveCameraRight(float distance);
        void moveCameraUp(float distance);
        void rotateCameraY(float distance);
        void rotateCamera(float x, float y);

        void setCamera(const Camera& camera);
    };
}
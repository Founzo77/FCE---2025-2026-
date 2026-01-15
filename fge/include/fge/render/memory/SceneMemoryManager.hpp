#pragma once

#include "TextureMemoryManager.hpp"
#include "MaterialMemoryManager.hpp"
#include "MeshMemoryManager.hpp"
#include "InstanceMemoryManager.hpp"
#include "LightMemoryManager.hpp"
#include "TextureBuffer.hpp"

#include "UploadBuffer.hpp"

#include "../indices.hpp"
#include "../data/Camera.hpp"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <unordered_map>
#include <memory>

using namespace Microsoft::WRL;
using std::unordered_map;
using std::shared_ptr;

namespace fge
{
    class SceneDescription;
    class DeviceConstantInformations;

    class Texture;
    class Material;
    class Mesh;
    class Instance;
    class Light;

    class RayTracingPipeline;
    class CbvSrvUavView;

    class SceneMemoryManager
    {
    public:
        TextureMemoryManager m_texturesMemory;
        MaterialMemoryManager m_materialsMemory;
        MeshMemoryManager m_meshesMemory;
        InstanceMemoryManager m_instancesMemory;
        LightMemoryManager m_lightsMemory;
        TextureBuffer m_outputTexture;

        Camera m_camera;
        bool m_constantBufferHasChanged;

        UploadBuffer m_constantUploadBuffer;

        shared_ptr<RayTracingPipeline> m_rayTracingPipeline;

    public:
        SceneMemoryManager() = default;
        ~SceneMemoryManager() = default;

        const MeshMemoryManager& getMeshMemory() const;
        
        Texture& getTexture(const LogicalIndex textureIndex);
        const Texture& getTexture(const LogicalIndex textureIndex) const;

        Material& getMaterial(const LogicalIndex materialIndex);
        const Material& getMaterial(const LogicalIndex materialIndex) const;

        Mesh& getMesh(const LogicalIndex meshIndex);
        const Mesh& getMesh(const LogicalIndex meshIndex) const;

        Instance& getInstance(const LogicalIndex instanceIndex);
        const Instance& getInstance(const LogicalIndex instanceIndex) const;

        Light& getLight(const LogicalIndex lightIndex);
        const Light& getLight(const LogicalIndex lightIndex) const;

        uint32_t getNbMeshes() const;
        uint32_t getNbSubMeshes() const;
        uint32_t getNbInstances() const;
        uint32_t getNbMaterials() const;
        uint32_t getNbTextures() const;
        uint32_t getNbLights() const;

        uint32_t getNbMaxMeshes() const;
        uint32_t getNbMaxSubMeshes() const;
        uint32_t getNbMaxInstances() const;
        uint32_t getNbMaxMaterials() const;
        uint32_t getNbMaxTextures() const;
        uint32_t getNbMaxLights() const;

        D3D12_GPU_VIRTUAL_ADDRESS getConstantUploadBufferAddress();

        ID3D12Resource* getOutputTextureBufferResource();
        ID3D12Resource* getVertexBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getIndexBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getSubMeshDataBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getLightBufferResource();
        ID3D12Resource* getMaterialBufferResource();
        ID3D12Resource* getIndirectionMaterialTableBufferResource();
        ID3D12Resource* getTextureBufferResource(const LogicalIndex textureIndex);

        D3D12_GPU_VIRTUAL_ADDRESS getOutputTextureBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getVertexBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getIndexBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getSubMeshesDataBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getLightBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getMaterialBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getIndirectionMaterialTableBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getTextureBufferAddress(const LogicalIndex textureIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getBlasBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getTlasBufferAddress();

        void initialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void initialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, SceneDescription& sceneDescription);
    private:
        void initializeConstantUploadBuffer(ComPtr<ID3D12Device5> device);

    public:

        void initializeOutputTexture(ComPtr<ID3D12Device5> device, 
            const uint32_t width, const uint32_t height);
        void resizeOutputTexture(ComPtr<ID3D12Device5> device, 
            const uint32_t width, const uint32_t height);
        
        void onCbsSrvUabViewInitializationCreateOutputUavTexture(
            ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
        void onCbsSrvUabViewInitializationFillMeshSrv(
            ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView);
        void onCbsSrvUabViewInitializationFillTextureSrv(
            ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView);

        void setCamera(const Camera& camera);
        void setRayTracingPipeline(shared_ptr<RayTracingPipeline> rayTracingPipeline);

        void updateDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
    private:
        void updateConstantBufferToDevice();
        void uploadConstantInformation(DeviceConstantInformations& constantInformation);

    public:
        
        void addLight(Light& newLight, const LogicalIndex lightIndex);
        void modifyLight(Light& modifiedLight, const LogicalIndex lightIndex);
        void deleteLight(const LogicalIndex lightIndex);

        void addInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void deleteInstance(const LogicalIndex instanceIndex);

    private:
        void initializeAddMesh(Mesh&& toAdd, const LogicalIndex meshIndex);
        void initializeAddInstance(Instance& toAdd, const LogicalIndex instanceIndex);
        void initializeAddTexture(Texture&& toAdd, const LogicalIndex textureIndex);
        void initializeAddMaterial(Material& toAdd, const LogicalIndex materialIndex);
        void initializeAddLight(Light& toAdd, const LogicalIndex lightIndex);
    };
}

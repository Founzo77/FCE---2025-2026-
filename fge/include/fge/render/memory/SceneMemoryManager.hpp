#pragma once

#include "TextureMemoryManager.hpp"
#include "MaterialMemoryManager.hpp"
#include "MeshMemoryManager.hpp"
#include "VolumeMemoryManager.hpp"
#include "InstanceMemoryManager.hpp"
#include "LightMemoryManager.hpp"
#include "TextureBuffer.hpp"

#include "UploadBuffer.hpp"

#include "../indices.hpp"
#include "../data/Background.hpp"
#include "../data/Camera.hpp"
#include "../data/DeviceConstantInformations.hpp"

#include "../RenderConfig.hpp"

#include "../../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <vector>

using namespace Microsoft::WRL;
using std::unordered_map;
using std::vector;
using std::shared_ptr;

namespace fge
{
    class SceneDescription;
    class DeviceConstantInformations;

    class Texture;
    class Material;
    class Mesh;
    class Volume;
    class Instance;
    class Light;

    class RayTracingPipeline;
    class CbvSrvUavView;

    class SceneMemoryManager
    {
    public:
        Background m_background;
        RenderConfig m_renderConfig;
        TextureMemoryManager m_texturesMemory;
        TextureMemoryManager m_textures3DMemory;
        MaterialMemoryManager m_materialsMemory;
        MeshMemoryManager m_meshesMemory;
        VolumeMemoryManager m_volumesMemory;
        InstanceMemoryManager m_instancesMemory;
        LightMemoryManager m_lightsMemory;
        TextureBuffer m_outputTexture;
        TextureBuffer m_accumulatedLeftTexture;
        TextureBuffer m_accumulatedRightTexture;
        bool m_isLeftAccumlatedInput;
        DeviceConstantInformations m_currentDeviceConstantInformations;
        ComPtr<ID3D12Resource> m_hostOutputBuffer;
        ComPtr<ID3D12Resource> m_sharedOutputTexture;
        HANDLE m_sharedOutputHandle{nullptr};

        Camera m_camera;
        bool m_constantBufferHasChanged;

        vector<UploadBuffer> m_constantUploadBuffers;

        shared_ptr<RayTracingPipeline> m_rayTracingPipeline;

    public:
        SceneMemoryManager() = default;
        ~SceneMemoryManager();

        const Background& getBackground() const noexcept;
        
        Texture& getTexture(const LogicalIndex textureIndex);
        const Texture& getTexture(const LogicalIndex textureIndex) const;

        Texture& getTexture3D(const LogicalIndex texture3DIndex);
        const Texture& getTexture3D(const LogicalIndex texture3DIndex) const;

        Material& getMaterial(const LogicalIndex materialIndex);
        const Material& getMaterial(const LogicalIndex materialIndex) const;

        Mesh& getMesh(const LogicalIndex meshIndex);
        const Mesh& getMesh(const LogicalIndex meshIndex) const;

        Volume& getVolume(const LogicalIndex volumeIndex);
        const Volume& getVolume(const LogicalIndex volumeIndex) const;

        Instance& getInstance(const LogicalIndex instanceIndex);
        const Instance& getInstance(const LogicalIndex instanceIndex) const;

        Light& getLight(const LogicalIndex lightIndex);
        const Light& getLight(const LogicalIndex lightIndex) const;

        uint32_t getNbMeshes() const;
        uint32_t getNbSubMeshes() const;
        uint32_t getNbVolumes() const;
        uint32_t getNbInstances() const;
        uint32_t getNbMaterials() const;
        uint32_t getNbTextures() const;
        uint32_t getNbTextures3D() const;
        uint32_t getNbLights() const;

        uint32_t getNbMaxMeshes() const;
        uint32_t getNbMaxSubMeshes() const;
        uint32_t getNbMaxVolumes() const;
        uint32_t getNbMaxInstances() const;
        uint32_t getNbMaxMaterials() const;
        uint32_t getNbMaxTextures() const;
        uint32_t getNbMaxTextures3D() const;
        uint32_t getNbMaxLights() const;

        D3D12_GPU_VIRTUAL_ADDRESS getConstantUploadBufferAddress(const uint32_t computeFrameIndex);

        ID3D12Resource* getOutputTextureBufferResource();
        ID3D12Resource* getAccumulatedLeftTextureBufferResource();
        ID3D12Resource* getAccumulatedRightTextureBufferResource();
        ID3D12Resource* getHostOutputTextureBufferResource();
        ID3D12Resource* getSharedOutputTextureBufferResource();
        HANDLE getSharedOutputTextureHandle();
        ID3D12Resource* getVertexBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getIndexBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getSubMeshDataBufferResource(const LogicalIndex meshIndex);
        ID3D12Resource* getVolumeDataBufferResource(const LogicalIndex volumeIndex);
        ID3D12Resource* getLightBufferResource();
        ID3D12Resource* getMaterialBufferResource();
        ID3D12Resource* getIndirectionMaterialTableBufferResource();
        ID3D12Resource* getTextureBufferResource(const LogicalIndex textureIndex);
        ID3D12Resource* getTexture3DBufferResource(const LogicalIndex texture3DIndex);

        D3D12_GPU_VIRTUAL_ADDRESS getOutputTextureBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getAccumulatedLeftTextureBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getAccumulatedRightTextureBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getVertexBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getIndexBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getSubMeshesDataBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getVolumeDataBufferAddress(const LogicalIndex volumeIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getLightBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getMaterialBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getIndirectionMaterialTableBufferAddress();
        D3D12_GPU_VIRTUAL_ADDRESS getTextureBufferAddress(const LogicalIndex textureIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getTexture3DBufferAddress(const LogicalIndex texture3DIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getBlasBufferAddress(const LogicalIndex meshIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getVolumeBlasBufferAddress(const LogicalIndex volumeIndex);
        D3D12_GPU_VIRTUAL_ADDRESS getTlasBufferAddress();

        bool isLeftAccumlatedInput() const noexcept;

        void initialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void initialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, SceneDescription& sceneDescription);
    private:
        void initializeConstantUploadBuffer(ComPtr<ID3D12Device5> device, const uint32_t nbComputeFrames);

    public:

        void initializeOutputTexture(ComPtr<ID3D12Device5> device, 
            const uint32_t width, const uint32_t height);
        void resizeOutputTexture(ComPtr<ID3D12Device5> device, 
            const uint32_t width, const uint32_t height);
        
        void onCbsSrvUabViewInitializationCreateOutputUavTexture(
            ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
        void onCbsSrvUabViewInitializationCreateAccumulatedLeftUavTexture(
            ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
        void onCbsSrvUabViewInitializationCreateAccumulatedRightUavTexture(
            ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
        void onCbsSrvUabViewInitializationFillMeshSrv(
            ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView);
        void onCbsSrvUabViewInitializationFillTextureSrv(
            ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView);
        void onCbsSrvUabViewInitializationFillTexture3DSrv(
            ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView);

        void reset();

        void setCamera(const Camera& camera);
        void setRayTracingPipeline(shared_ptr<RayTracingPipeline> rayTracingPipeline);

        void updateDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, const uint32_t computeFrameIndex);
    private:
        void updateConstantBufferToDevice(const uint32_t computeFrameIndex);
        void uploadConstantInformation(DeviceConstantInformations& constantInformation,
            const uint32_t computeFrameIndex);

    public:
        
        void addLight(Light& newLight, const LogicalIndex lightIndex);
        void modifyLight(Light& modifiedLight, const LogicalIndex lightIndex);
        void deleteLight(const LogicalIndex lightIndex);

        void addInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex);
        void deleteInstance(const LogicalIndex instanceIndex);

    private:
        void initializeAddMesh(Mesh&& toAdd, const LogicalIndex meshIndex);
        void initializeAddVolume(Volume&& toAdd, const LogicalIndex volumeIndex);
        void initializeAddInstance(Instance& toAdd, const LogicalIndex instanceIndex);
        void initializeAddTexture(Texture&& toAdd, const LogicalIndex textureIndex);
        void initializeAddTexture3D(Texture&& toAdd, const LogicalIndex texture3DIndex);
        void initializeAddMaterial(Material& toAdd, const LogicalIndex materialIndex);
        void initializeAddLight(Light& toAdd, const LogicalIndex lightIndex);
    };
}

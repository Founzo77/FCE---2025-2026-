#include <fge/render/memory/SceneMemoryManager.hpp>

#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/Volume.hpp>
#include <fge/render/data/Instance.hpp>
#include <fge/render/data/Material.hpp>
#include <fge/render/data/Texture.hpp>

#include <fge/render/pipeline_state/RayTracingPipeline.hpp>

#include <fge/utility.hpp>

#include <fge/io/SceneDescription.hpp>

#include <d3dx12.h>

namespace fge
{
    SceneMemoryManager::~SceneMemoryManager()
    {
        reset();
    }

    const Background& SceneMemoryManager::getBackground() const noexcept
    {
        return m_background;
    }

    Texture& SceneMemoryManager::getTexture(const LogicalIndex textureIndex)
    {
        return m_texturesMemory.get(textureIndex);
    }

    const Texture& SceneMemoryManager::getTexture(const LogicalIndex textureIndex) const
    {
        return m_texturesMemory.get(textureIndex);
    }

    Texture& SceneMemoryManager::getTexture3D(const LogicalIndex texture3DIndex)
    {
        return m_textures3DMemory.get(texture3DIndex);
    }

    const Texture& SceneMemoryManager::getTexture3D(const LogicalIndex texture3DIndex) const
    {
        return m_textures3DMemory.get(texture3DIndex);
    }

    Material& SceneMemoryManager::getMaterial(const LogicalIndex materialIndex)
    {
        return m_materialsMemory.get(materialIndex);
    }

    const Material& SceneMemoryManager::getMaterial(const LogicalIndex materialIndex) const
    {
        return m_materialsMemory.get(materialIndex);
    }

    Mesh& SceneMemoryManager::getMesh(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex);
    }

    const Mesh& SceneMemoryManager::getMesh(const LogicalIndex meshIndex) const
    {
        return m_meshesMemory.get(meshIndex);
    }

    Volume& SceneMemoryManager::getVolume(const LogicalIndex volumeIndex)
    {
        return m_volumesMemory.get(volumeIndex);
    }

    const Volume& SceneMemoryManager::getVolume(const LogicalIndex volumeIndex) const
    {
        return m_volumesMemory.get(volumeIndex);
    }

    Instance& SceneMemoryManager::getInstance(const LogicalIndex instanceIndex)
    {
        return m_instancesMemory.get(instanceIndex);
    }

    const Instance& SceneMemoryManager::getInstance(const LogicalIndex instanceIndex) const
    {
        return m_instancesMemory.get(instanceIndex);
    }

    Light& SceneMemoryManager::getLight(const LogicalIndex lightIndex)
    {
        return m_lightsMemory.get(lightIndex);
    }

    const Light& SceneMemoryManager::getLight(const LogicalIndex lightIndex) const
    {
        return m_lightsMemory.get(lightIndex);
    }

    uint32_t SceneMemoryManager::getNbMeshes() const
    {
        return m_meshesMemory.getNbMeshes();
    }

    uint32_t SceneMemoryManager::getNbSubMeshes() const
    {
        return m_meshesMemory.getNbSubMeshes();
    }

    uint32_t SceneMemoryManager::getNbVolumes() const
    {
        return m_volumesMemory.getNbVolumes();
    }

    uint32_t SceneMemoryManager::getNbInstances() const
    {
        return m_instancesMemory.getNbInstances();
    }

    uint32_t SceneMemoryManager::getNbMaterials() const
    {
        return m_materialsMemory.getNbMaterials();
    }

    uint32_t SceneMemoryManager::getNbTextures() const
    {
        return m_texturesMemory.getNbTextures();
    }

    uint32_t SceneMemoryManager::getNbTextures3D() const
    {
        return m_textures3DMemory.getNbTextures();
    }

    uint32_t SceneMemoryManager::getNbLights() const
    {
        return m_lightsMemory.getNbLights();
    }

    uint32_t SceneMemoryManager::getNbMaxMeshes() const
    {
        return m_meshesMemory.getNbMaxMeshes();
    }

    uint32_t SceneMemoryManager::getNbMaxSubMeshes() const
    {
        return m_meshesMemory.getNbMaxSubMeshes();
    }

    uint32_t SceneMemoryManager::getNbMaxVolumes() const
    {
        return m_volumesMemory.getNbMaxVolumes();
    }

    uint32_t SceneMemoryManager::getNbMaxInstances() const
    {
        return m_instancesMemory.getNbMaxInstances();
    }

    uint32_t SceneMemoryManager::getNbMaxMaterials() const
    {
        return m_materialsMemory.getNbMaxMaterials();
    }

    uint32_t SceneMemoryManager::getNbMaxTextures() const
    {
        return m_texturesMemory.getNbMaxTextures();
    }

    uint32_t SceneMemoryManager::getNbMaxTextures3D() const
    {
        return m_textures3DMemory.getNbMaxTextures();
    }

    uint32_t SceneMemoryManager::getNbMaxLights() const
    {
        return m_lightsMemory.getNbMaxLights();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getConstantUploadBufferAddress(
        const uint32_t computeFrameIndex)
    {
        return m_constantUploadBuffers[computeFrameIndex].getBuffer()->GetGPUVirtualAddress();
    }

    ID3D12Resource* SceneMemoryManager::getOutputTextureBufferResource()
    {
        return m_outputTexture.getTexture().Get();
    }

    ID3D12Resource* SceneMemoryManager::getAccumulatedLeftTextureBufferResource()
    {
        return m_accumulatedLeftTexture.getTexture().Get();
    }

    ID3D12Resource* SceneMemoryManager::getAccumulatedRightTextureBufferResource()
    {
        return m_accumulatedRightTexture.getTexture().Get();
    }

    ID3D12Resource* SceneMemoryManager::getHostOutputTextureBufferResource()
    {
        return m_hostOutputBuffer.Get();
    }

    ID3D12Resource* SceneMemoryManager::getSharedOutputTextureBufferResource()
    {
        return m_sharedOutputTexture.Get();
    }

    HANDLE SceneMemoryManager::getSharedOutputTextureHandle()
    {
        return m_sharedOutputHandle;
    }

    ID3D12Resource* SceneMemoryManager::getVertexBufferResource(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_vertexBuffer.getBuffer().Get();
    }

    ID3D12Resource* SceneMemoryManager::getIndexBufferResource(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_indexBuffer.getBuffer().Get();
    }

    ID3D12Resource* SceneMemoryManager::getSubMeshDataBufferResource(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_subMeshesDataBuffer.getBuffer().Get();
    }

    ID3D12Resource* SceneMemoryManager::getVolumeDataBufferResource(const LogicalIndex volumeIndex)
    {
        return getVolume(volumeIndex).m_volumeDataBuffer.getBuffer().Get();
    }

    ID3D12Resource* SceneMemoryManager::getLightBufferResource()
    {
        return m_lightsMemory.m_deviceBuffer.Get();
    }

    ID3D12Resource* SceneMemoryManager::getMaterialBufferResource()
    {
        return m_materialsMemory.m_deviceBuffer.Get();
    }

    ID3D12Resource* SceneMemoryManager::getIndirectionMaterialTableBufferResource()
    {
        return m_materialsMemory.m_indirectionTableDeviceBuffer.getBuffer().Get();
    }

    ID3D12Resource* SceneMemoryManager::getTextureBufferResource(const LogicalIndex textureIndex)
    {
        return m_texturesMemory.get(textureIndex).m_deviceBuffer.getTexture().Get();
    }

    ID3D12Resource* SceneMemoryManager::getTexture3DBufferResource(const LogicalIndex texture3DIndex)
    {
        return m_textures3DMemory.get(texture3DIndex).m_deviceBuffer.getTexture().Get();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getOutputTextureBufferAddress()
    {
        return m_outputTexture.getTexture()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getAccumulatedLeftTextureBufferAddress()
    {
        return m_accumulatedLeftTexture.getTexture()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getAccumulatedRightTextureBufferAddress()
    {
        return m_accumulatedRightTexture.getTexture()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getVertexBufferAddress(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_vertexBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getIndexBufferAddress(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_indexBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getSubMeshesDataBufferAddress(
        const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_subMeshesDataBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getVolumeDataBufferAddress(
        const LogicalIndex volumeIndex)
    {
        return getVolume(volumeIndex).m_volumeDataBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getLightBufferAddress()
    {
        return m_lightsMemory.m_deviceBuffer->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getMaterialBufferAddress()
    {
        return m_materialsMemory.m_deviceBuffer->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getIndirectionMaterialTableBufferAddress()
    {
        return m_materialsMemory.m_indirectionTableDeviceBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getTextureBufferAddress(
        const LogicalIndex textureIndex)
    {
        return m_texturesMemory.get(textureIndex).m_deviceBuffer.getTexture()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getTexture3DBufferAddress(
        const LogicalIndex texture3DIndex)
    {
        return m_textures3DMemory.get(texture3DIndex).m_deviceBuffer.getTexture()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getBlasBufferAddress(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_blasBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS fge::SceneMemoryManager::getVolumeBlasBufferAddress(
        const LogicalIndex volumeIndex)
    {
        return getVolume(volumeIndex).m_blasBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getTlasBufferAddress()
    {
        return m_instancesMemory.getTlasBufferAddress();
    }

    bool SceneMemoryManager::isLeftAccumlatedInput() const noexcept
    {
        return m_isLeftAccumlatedInput;
    }

    void SceneMemoryManager::initialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        // TO_DO
        throwIfFailed(true == false, "Unimplamented");
    }

    void SceneMemoryManager::initialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList, SceneDescription& sceneDescription)
    {
        m_background = sceneDescription.m_background;
        m_renderConfig = std::get<RenderConfig>(sceneDescription.m_renderConfig);

        m_texturesMemory.startInitialize(device, directCommandList);
        m_textures3DMemory.startInitialize(device, directCommandList);
        m_materialsMemory.startInitialize(device, directCommandList);
        m_meshesMemory.startInitialize(device, directCommandList);
        m_volumesMemory.startInitialize(device, directCommandList);
        m_instancesMemory.startInitialize();
        m_lightsMemory.startInitialize(device, directCommandList);

        if(sceneDescription.m_volumes.size() == 0)
        {
            Volume volume;
            volume.m_hitGroupIndex = 0;

            VolumeData volumeData;
            volumeData.m_mins = { 0, 0, 0 };
            volumeData.m_maxs = { 1, 1, 1 };
            volumeData.m_scalarMin = 0;
            volumeData.m_scalarMax = 1;
            volumeData.m_texture3DIndex = UINT32_MAX;
            volumeData.m_transferFunctionTextureIndex = UINT32_MAX;

            volume.m_volumeData = volumeData;

            sceneDescription.m_volumes[0] = std::move(volume);
        }

        if(sceneDescription.m_lights.size() == 0)
        {
            Light light;

            light.m_radiance = { 0, 0, 0 };
            light.m_position = { 10000, 10000, 10000 };

            sceneDescription.m_lights.push_back(std::move(light));
        }

        for(uint64_t i = 0; i < sceneDescription.m_textures.size(); i++)
        {
            initializeAddTexture(std::move(sceneDescription.m_textures[i]), i);
        }

        for(uint64_t i = 0; i < sceneDescription.m_textures3D.size(); i++)
        {
            initializeAddTexture3D(std::move(sceneDescription.m_textures3D[i]), i);
        }

        for(uint64_t i = 0; i < sceneDescription.m_materials.size(); i++)
        {
            initializeAddMaterial(sceneDescription.m_materials[i], i);
        }

        for(auto& [index, mesh] : sceneDescription.m_meshes)
        {
            initializeAddMesh(std::move(mesh), index);
        }

        for(auto& [index, volume] : sceneDescription.m_volumes)
        {
            initializeAddVolume(std::move(volume), index);
        }

        for(auto& [index, instance] : sceneDescription.m_instances)
        {
            initializeAddInstance(instance, index);
        }

        for(uint64_t i = 0; i < sceneDescription.m_lights.size(); i++)
        {
            initializeAddLight(sceneDescription.m_lights[i], i);
        }

        m_camera = std::move(sceneDescription.m_camera);
        m_constantBufferHasChanged = true;

        // TO_DO : Log de la scene

        m_texturesMemory.moveAllToDevice(device, directCommandList);
        m_textures3DMemory.moveAllToDevice(device, directCommandList);
        m_materialsMemory.moveAllToDevice(device, directCommandList);
        m_meshesMemory.moveAllToDevice(device, directCommandList);
        m_volumesMemory.moveAllToDevice(device, directCommandList);
        m_lightsMemory.moveAllToDevice(device, directCommandList);
        m_instancesMemory.moveAllToDevice(device, directCommandList, m_meshesMemory, m_volumesMemory);

        m_texturesMemory.endInitialize(device, directCommandList);
        m_textures3DMemory.endInitialize(device, directCommandList);
        m_materialsMemory.endInitialize(device, directCommandList);
        m_meshesMemory.endInitialize(device, directCommandList);
        m_volumesMemory.endInitialize(device, directCommandList);
        m_instancesMemory.endInitialize();
        m_lightsMemory.endInitialize(device, directCommandList);

        // TO_DO voir si pas juste passer m_renderConfig directement
        initializeConstantUploadBuffer(device, m_renderConfig.m_nbComputeFrames);
    }

    void SceneMemoryManager::initializeConstantUploadBuffer(ComPtr<ID3D12Device5> device, 
        const uint32_t nbComputeFrames)
    {
        uint32_t constantUploadBufferSize = alignData(sizeof(DeviceConstantInformations),
            D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        m_constantUploadBuffers.resize(nbComputeFrames);
        for(UploadBuffer& constantUploadBuffer : m_constantUploadBuffers)
        {
            constantUploadBuffer.initialize(device, constantUploadBufferSize);
            d12SetDebugName(constantUploadBuffer.m_buffer, L"Constant Upload Buffer");
        }
    }

    void SceneMemoryManager::initializeOutputTexture(ComPtr<ID3D12Device5> device, 
        const uint32_t width, const uint32_t height)
    {
        m_outputTexture.initialize(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R8G8B8A8_UNORM);

        m_accumulatedLeftTexture.initialize(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R32G32B32A32_FLOAT);
        m_accumulatedRightTexture.initialize(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R32G32B32A32_FLOAT);

        // TO_DO function initiliaze : m_hostOutputBuffer
        ID3D12Resource* output = getOutputTextureBufferResource();
        D3D12_RESOURCE_DESC desc = output->GetDesc();

        uint64_t totalSize = 0;

        device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &totalSize);

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_READBACK;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = totalSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        throwIfFailed(device->CreateCommittedResource(&heapProps, 
            D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, IID_PPV_ARGS(&m_hostOutputBuffer)));

        // texture partagée
        {
            ID3D12Resource* output = getOutputTextureBufferResource();
            D3D12_RESOURCE_DESC desc = output->GetDesc();

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

            throwIfFailed(device->CreateCommittedResource(&heapProps,
                D3D12_HEAP_FLAG_SHARED, &desc, D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr, IID_PPV_ARGS(&m_sharedOutputTexture)),
                "Failed to create shared output texture");

            throwIfFailed(device->CreateSharedHandle(m_sharedOutputTexture.Get(),
                nullptr, GENERIC_ALL, nullptr, &m_sharedOutputHandle),
                "Failed to create shared handle");
        }
    }

    void SceneMemoryManager::resizeOutputTexture(ComPtr<ID3D12Device5> device, 
        const uint32_t width, const uint32_t height)
    {
        m_outputTexture.reallocate(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R8G8B8A8_UNORM);

        m_accumulatedLeftTexture.reallocate(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R32G32B32A32_FLOAT);
        m_accumulatedRightTexture.reallocate(device, width, height,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, DXGI_FORMAT_R32G32B32A32_FLOAT);

        ID3D12Resource* output = getOutputTextureBufferResource();
        D3D12_RESOURCE_DESC desc = output->GetDesc();

        uint64_t totalSize = 0;

        device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &totalSize);

        m_hostOutputBuffer.Reset();

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_READBACK;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = totalSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        throwIfFailed(device->CreateCommittedResource(&heapProps,
            D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr, IID_PPV_ARGS(&m_hostOutputBuffer)));
        

        // texture partagée
        {
            m_sharedOutputTexture.Reset();
            if (m_sharedOutputHandle)
            {
                CloseHandle(m_sharedOutputHandle);
                m_sharedOutputHandle = nullptr;
            }
            
            ID3D12Resource* output = getOutputTextureBufferResource();
            D3D12_RESOURCE_DESC desc = output->GetDesc();

            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

            throwIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_SHARED, 
                &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                IID_PPV_ARGS(&m_sharedOutputTexture)), "Failed to create shared output texture");

            throwIfFailed(device->CreateSharedHandle(m_sharedOutputTexture.Get(),
                nullptr, GENERIC_ALL, nullptr, &m_sharedOutputHandle),
                "Failed to create shared handle");
        }
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationCreateOutputUavTexture(
        ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        m_outputTexture.createUAV(device, cpuHandle);
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationCreateAccumulatedLeftUavTexture(
        ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        m_accumulatedLeftTexture.createUAV(device, cpuHandle);
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationCreateAccumulatedRightUavTexture(
        ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        m_accumulatedRightTexture.createUAV(device, cpuHandle);
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationFillMeshSrv(
        ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView)
    {
        uint32_t subMeshIndex = 0;

        for(auto meshIndex : m_meshesMemory.iterateOverIndexMeshes())
        {
            LogicalIndex logicalMeshIndex = m_meshesMemory.physicalToLogicalIndex(meshIndex.index);

            handleView.setVertexSrvHandle(device, 
                logicalMeshIndex.m_index, meshIndex.value.m_vertices.size(),
                getVertexBufferResource(logicalMeshIndex.m_index));

            for(SubMesh& subMesh : meshIndex.value.m_subMeshes)
            {
                handleView.setIndexSrvHandle(device, subMeshIndex, subMesh.m_nbIndices,
                    getIndexBufferResource(logicalMeshIndex.m_index), subMesh.m_startIndices);
                subMeshIndex++;
            }
        }
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationFillTextureSrv(
        ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView)
    {
        uint32_t textureIndex = 0;

        // TO_DO Iterate over texture index
        for(Texture& texture : m_texturesMemory.iterateOverTextures())
        {
            handleView.setTextureSrvHandle(device, textureIndex, 
                getTextureBufferResource(textureIndex));

            textureIndex++;
        }
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationFillTexture3DSrv(
        ComPtr<ID3D12Device5> device, CbvSrvUavView& handleView)
    {
        uint32_t texture3DIndex = 0;

        // TO_DO Iterate over texture index
        for(Texture& texture : m_textures3DMemory.iterateOverTextures())
        {
            handleView.setTexture3DSrvHandle(device, texture3DIndex, 
                getTexture3DBufferResource(texture3DIndex));

            texture3DIndex++;
        }
    }

    void SceneMemoryManager::reset()
    {
        m_camera = Camera{};
        m_constantBufferHasChanged = false;

        m_outputTexture.reset();
        m_accumulatedLeftTexture.reset();
        m_accumulatedRightTexture.reset();
        m_hostOutputBuffer.Reset();
        m_sharedOutputTexture.Reset();
        if(m_sharedOutputHandle)
        {
            CloseHandle(m_sharedOutputHandle);
            m_sharedOutputHandle = nullptr;
        }

        for(UploadBuffer& constantUploadBuffer : m_constantUploadBuffers)
        {
            constantUploadBuffer.reset();
        }
        m_constantUploadBuffers.clear();

        m_texturesMemory.reset();
        m_textures3DMemory.reset();
        m_materialsMemory.reset();
        m_meshesMemory.reset();
        m_volumesMemory.reset();
        m_instancesMemory.reset();
        m_lightsMemory.reset();

        m_rayTracingPipeline.reset();
    }

    void SceneMemoryManager::setCamera(const Camera& camera)
    {
        if(m_camera != camera)
        {
            m_camera = camera;
            m_constantBufferHasChanged = true;
        }
    }

    void SceneMemoryManager::setRayTracingPipeline(shared_ptr<RayTracingPipeline> rayTracingPipeline)
    {
        m_rayTracingPipeline = rayTracingPipeline;
    }

    void SceneMemoryManager::updateDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList, const uint32_t computeFrameIndex)
    {
        m_lightsMemory.updateDevice(directCommandList);

        if(m_lightsMemory.m_hasLightsResized)
        {
            m_lightsMemory.m_hasLightsResized = false;
            m_constantBufferHasChanged = true;
        }

        m_instancesMemory.updateDevice(device, directCommandList, m_meshesMemory, m_volumesMemory);

        updateConstantBufferToDevice(computeFrameIndex);
    }

    void SceneMemoryManager::updateConstantBufferToDevice(const uint32_t computeFrameIndex)
    {
        m_isLeftAccumlatedInput = !m_isLeftAccumlatedInput;

        if(m_constantBufferHasChanged)
        {
            DeviceCamera deviceCamera(m_camera);
            m_currentDeviceConstantInformations.m_position = deviceCamera.m_position;
            m_currentDeviceConstantInformations.m_forward = deviceCamera.m_forward;
            m_currentDeviceConstantInformations.m_up = deviceCamera.m_up;
            m_currentDeviceConstantInformations.m_right = deviceCamera.m_right;
            m_currentDeviceConstantInformations.m_physicalWidth = deviceCamera.m_physicalWidth;
            m_currentDeviceConstantInformations.m_physicalHeight = deviceCamera.m_physicalHeight;
            m_currentDeviceConstantInformations.m_depth = deviceCamera.m_depth;
            m_currentDeviceConstantInformations.m_nbLights = getNbLights();
            m_currentDeviceConstantInformations.m_backgroundTextureIndex = 
                getBackground().m_textureIndex;
            m_currentDeviceConstantInformations.m_integratorType = m_renderConfig.m_integratorType;
            m_currentDeviceConstantInformations.m_spp = m_renderConfig.m_spp;

            m_currentDeviceConstantInformations.m_sppIndex = 0;
            
            uploadConstantInformation(m_currentDeviceConstantInformations, computeFrameIndex);
            m_constantBufferHasChanged = false;
        }
        else
        {
            m_currentDeviceConstantInformations.m_sppIndex += m_renderConfig.m_spp;
            // TO_DO voir si upload de juste sppIndex worth
            uploadConstantInformation(m_currentDeviceConstantInformations, computeFrameIndex);
        }
    }

    void SceneMemoryManager::uploadConstantInformation(DeviceConstantInformations& constantInformation,
        const uint32_t computeFrameIndex)
    {
        m_constantUploadBuffers[computeFrameIndex].upload(
            &constantInformation, sizeof(DeviceConstantInformations));
    }

    void SceneMemoryManager::addLight(Light& newLight, const LogicalIndex lightIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        m_lightsMemory.add(newLight, lightIndex);
    }

    void SceneMemoryManager::modifyLight(Light& modifiedLight, const LogicalIndex lightIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        m_lightsMemory.modify(lightIndex, modifiedLight);
    }

    void SceneMemoryManager::deleteLight(const LogicalIndex lightIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        m_lightsMemory.free(lightIndex);
    }

    void SceneMemoryManager::addInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        m_instancesMemory.add(newInstance, instanceIndex);

        if(newInstance.m_geometryReference.m_type == GeometryType::TRIANGLES)
        {
            m_meshesMemory.share(newInstance.m_geometryReference.m_geometryIndex);
        }
        else if(newInstance.m_geometryReference.m_type == GeometryType::AABB)
        {
            m_volumesMemory.share(newInstance.m_geometryReference.m_geometryIndex);
        }
    }

    void SceneMemoryManager::modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        m_instancesMemory.modify(instanceIndex, newInstance);
    }

    void SceneMemoryManager::deleteInstance(const LogicalIndex instanceIndex)
    {
        m_constantBufferHasChanged = true; // TO_DO faire sppIndexhasChanged ?

        const Instance& old = m_instancesMemory.get(instanceIndex);

        if(old.m_geometryReference.m_type == GeometryType::TRIANGLES)
        {
            m_meshesMemory.share(old.m_geometryReference.m_geometryIndex);
        }
        else if(old.m_geometryReference.m_type == GeometryType::AABB)
        {
            m_volumesMemory.share(old.m_geometryReference.m_geometryIndex);
        }

        m_instancesMemory.free(instanceIndex);
    }

    void SceneMemoryManager::initializeAddMesh(Mesh&& toAdd, const LogicalIndex meshIndex)
    {
        m_meshesMemory.add(std::move(toAdd), meshIndex);

        Mesh& currentMesh = m_meshesMemory.get(meshIndex);
        for(SubMeshData& subMeshData : currentMesh.m_subMeshesData)
        {
            m_materialsMemory.share(subMeshData.m_materialIndex);
        }
    }

    void SceneMemoryManager::initializeAddVolume(Volume&& toAdd, const LogicalIndex volumeIndex)
    {
        m_volumesMemory.add(std::move(toAdd), volumeIndex);

        Volume& currentVolume = getVolume(volumeIndex);
        if(currentVolume.m_volumeData.m_texture3DIndex != UINT32_MAX)
            m_textures3DMemory.share(currentVolume.m_volumeData.m_texture3DIndex);
        // TO_DO Voir comment gerer les UINT32_MAX pour les indices de manieres uniformes
        if(currentVolume.m_volumeData.m_transferFunctionTextureIndex != UINT32_MAX)
            m_texturesMemory.share(currentVolume.m_volumeData.m_transferFunctionTextureIndex);
    }

    void SceneMemoryManager::initializeAddInstance(Instance& toAdd, 
        const LogicalIndex instanceIndex)
    {
        m_instancesMemory.add(toAdd, instanceIndex);

        Instance& currentInstance = m_instancesMemory.get(instanceIndex);

        if(currentInstance.m_geometryReference.m_type == GeometryType::TRIANGLES)
        {
            m_meshesMemory.share(currentInstance.m_geometryReference.m_geometryIndex);
        }
        else if(currentInstance.m_geometryReference.m_type == GeometryType::AABB)
        {
            m_volumesMemory.share(currentInstance.m_geometryReference.m_geometryIndex);
        }
    }

    void SceneMemoryManager::initializeAddTexture(Texture&& toAdd, const LogicalIndex textureIndex)
    {
        m_texturesMemory.add(std::move(toAdd), textureIndex);
    }

    void SceneMemoryManager::initializeAddTexture3D(
        Texture&& toAdd, const LogicalIndex texture3DIndex)
    {
        m_textures3DMemory.add(std::move(toAdd), texture3DIndex);
    }

    void SceneMemoryManager::initializeAddMaterial(Material& toAdd, 
        const LogicalIndex materialIndex)
    {
        m_materialsMemory.add(toAdd, materialIndex);
        
        Material& currentMaterial = m_materialsMemory.get(materialIndex);
        if(currentMaterial.m_albedoTextureIndex != UINT32_MAX)
        {
            m_texturesMemory.share(currentMaterial.m_albedoTextureIndex);
        }
        if(currentMaterial.m_normalTextureIndex != UINT32_MAX)
        {
            m_texturesMemory.share(currentMaterial.m_normalTextureIndex);
        }
    }

    void SceneMemoryManager::initializeAddLight(Light& toAdd, 
        const LogicalIndex lightIndex)
    {
        m_lightsMemory.add(toAdd, lightIndex);
    }
}

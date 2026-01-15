#include <fge/render/memory/SceneMemoryManager.hpp>

#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/Instance.hpp>
#include <fge/render/data/Material.hpp>
#include <fge/render/data/Texture.hpp>
#include <fge/render/data/DeviceConstantInformations.hpp>

#include <fge/render/pipeline_state/RayTracingPipeline.hpp>

#include <fge/utility.hpp>

#include <fge/io/SceneDescription.hpp>

#include <d3dx12.h>

namespace fge
{
    const MeshMemoryManager& SceneMemoryManager::getMeshMemory() const
    {
        return m_meshesMemory;
    }

    Texture& SceneMemoryManager::getTexture(const LogicalIndex textureIndex)
    {
        return m_texturesMemory.get(textureIndex);
    }

    const Texture& SceneMemoryManager::getTexture(const LogicalIndex textureIndex) const
    {
        return m_texturesMemory.get(textureIndex);
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

    uint32_t fge::SceneMemoryManager::getNbMeshes() const
    {
        return m_meshesMemory.getNbMeshes();
    }

    uint32_t fge::SceneMemoryManager::getNbSubMeshes() const
    {
        return m_meshesMemory.getNbSubMeshes();
    }

    uint32_t fge::SceneMemoryManager::getNbInstances() const
    {
        return m_instancesMemory.getNbInstances();
    }

    uint32_t fge::SceneMemoryManager::getNbMaterials() const
    {
        return m_materialsMemory.getNbMaterials();
    }

    uint32_t fge::SceneMemoryManager::getNbTextures() const
    {
        return m_texturesMemory.getNbTextures();
    }

    uint32_t fge::SceneMemoryManager::getNbLights() const
    {
        return m_lightsMemory.getNbLights();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxMeshes() const
    {
        return m_meshesMemory.getNbMaxMeshes();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxSubMeshes() const
    {
        return m_meshesMemory.getNbMaxSubMeshes();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxInstances() const
    {
        return m_instancesMemory.getNbMaxInstances();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxMaterials() const
    {
        return m_materialsMemory.getNbMaxMaterials();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxTextures() const
    {
        return m_texturesMemory.getNbMaxTextures();
    }

    uint32_t fge::SceneMemoryManager::getNbMaxLights() const
    {
        return m_lightsMemory.getNbMaxLights();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getConstantUploadBufferAddress()
    {
        return m_constantUploadBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    ID3D12Resource* SceneMemoryManager::getOutputTextureBufferResource()
    {
        return m_outputTexture.getTexture().Get();
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

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getOutputTextureBufferAddress()
    {
        return m_outputTexture.getTexture()->GetGPUVirtualAddress();
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

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getBlasBufferAddress(const LogicalIndex meshIndex)
    {
        return m_meshesMemory.get(meshIndex).m_blasBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    D3D12_GPU_VIRTUAL_ADDRESS SceneMemoryManager::getTlasBufferAddress()
    {
        return m_instancesMemory.getTlasBufferAddress();
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
        m_texturesMemory.startInitialize(device, directCommandList);
        m_materialsMemory.startInitialize(device, directCommandList);
        m_meshesMemory.startInitialize(device, directCommandList);
        m_instancesMemory.startInitialize();
        m_lightsMemory.startInitialize(device, directCommandList);

        for(uint64_t i = 0; i < sceneDescription.m_textures.size(); i++)
        {
            initializeAddTexture(std::move(sceneDescription.m_textures[i]), i);
        }

        for(uint64_t i = 0; i < sceneDescription.m_materials.size(); i++)
        {
            initializeAddMaterial(sceneDescription.m_materials[i], i);
        }

        for(auto& [index, mesh] : sceneDescription.m_meshes)
        {
            initializeAddMesh(std::move(mesh), index);
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
        m_materialsMemory.moveAllToDevice(device, directCommandList);
        m_meshesMemory.moveAllToDevice(device, directCommandList);
        m_lightsMemory.moveAllToDevice(device, directCommandList);
        m_instancesMemory.moveAllToDevice(device, directCommandList, m_meshesMemory);

        m_texturesMemory.endInitialize(device, directCommandList);
        m_materialsMemory.endInitialize(device, directCommandList);
        m_meshesMemory.endInitialize(device, directCommandList);
        m_instancesMemory.endInitialize();
        m_lightsMemory.endInitialize(device, directCommandList);

        initializeConstantUploadBuffer(device);
    }

    void SceneMemoryManager::initializeConstantUploadBuffer(ComPtr<ID3D12Device5> device)
    {
        uint32_t constantUploadBufferSize = alignData(sizeof(DeviceConstantInformations),
            D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
        m_constantUploadBuffer.initialize(device, constantUploadBufferSize);
    }

    void SceneMemoryManager::initializeOutputTexture(ComPtr<ID3D12Device5> device, 
        const uint32_t width, const uint32_t height)
    {
        m_outputTexture.initialize(device, width, height);
    }

    void SceneMemoryManager::resizeOutputTexture(ComPtr<ID3D12Device5> device, 
        const uint32_t width, const uint32_t height)
    {
        m_outputTexture.reallocate(device, width, height);
    }

    void SceneMemoryManager::onCbsSrvUabViewInitializationCreateOutputUavTexture(
        ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        m_outputTexture.createUAV(device, cpuHandle);
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

    void SceneMemoryManager::setCamera(const Camera& camera)
    {
        m_camera = camera;
        m_constantBufferHasChanged = true;
    }

    void SceneMemoryManager::setRayTracingPipeline(shared_ptr<RayTracingPipeline> rayTracingPipeline)
    {
        m_rayTracingPipeline = rayTracingPipeline;
    }

    void SceneMemoryManager::updateDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_lightsMemory.updateDevice(directCommandList);

        if(m_lightsMemory.m_hasLightsResized)
        {
            m_lightsMemory.m_hasLightsResized = false;
            m_constantBufferHasChanged = true;
        }

        m_instancesMemory.updateDevice(device, directCommandList, m_meshesMemory);

        updateConstantBufferToDevice();
    }

    void SceneMemoryManager::updateConstantBufferToDevice()
    {
        if(m_constantBufferHasChanged)
        {
            DeviceCamera deviceCamera(m_camera);
            DeviceConstantInformations deviceConstantInformation;
            deviceConstantInformation.m_position = deviceCamera.m_position;
            deviceConstantInformation.m_forward = deviceCamera.m_forward;
            deviceConstantInformation.m_up = deviceCamera.m_up;
            deviceConstantInformation.m_right = deviceCamera.m_right;
            deviceConstantInformation.m_physicalWidth = deviceCamera.m_physicalWidth;
            deviceConstantInformation.m_physicalHeight = deviceCamera.m_physicalHeight;
            deviceConstantInformation.m_depth = deviceCamera.m_depth;
            deviceConstantInformation.m_nbLights = getNbLights();
            
            uploadConstantInformation(deviceConstantInformation);

            m_constantBufferHasChanged = false;
        }
    }

    void SceneMemoryManager::uploadConstantInformation(DeviceConstantInformations& constantInformation)
    {
        m_constantUploadBuffer.upload(&constantInformation, sizeof(DeviceConstantInformations));
    }

    void fge::SceneMemoryManager::addLight(Light& newLight, const LogicalIndex lightIndex)
    {
        m_lightsMemory.add(newLight, lightIndex);
    }

    void SceneMemoryManager::modifyLight(Light& modifiedLight, const LogicalIndex lightIndex)
    {
        m_lightsMemory.modify(lightIndex, modifiedLight);
    }

    void SceneMemoryManager::deleteLight(const LogicalIndex lightIndex)
    {
        m_lightsMemory.free(lightIndex);
    }

    void SceneMemoryManager::addInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_instancesMemory.add(newInstance, instanceIndex);
        m_meshesMemory.share(newInstance.m_meshIndex);
    }

    void SceneMemoryManager::modifyInstance(Instance& newInstance, const LogicalIndex instanceIndex)
    {
        m_instancesMemory.modify(instanceIndex, newInstance);
    }

    void SceneMemoryManager::deleteInstance(const LogicalIndex instanceIndex)
    {
        const Instance& old = m_instancesMemory.get(instanceIndex);
        m_meshesMemory.free(old.m_meshIndex);
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

    void SceneMemoryManager::initializeAddInstance(Instance& toAdd, 
        const LogicalIndex instanceIndex)
    {
        m_instancesMemory.add(toAdd, instanceIndex);

        Instance& currentInstance = m_instancesMemory.get(instanceIndex);
        m_meshesMemory.share(currentInstance.m_meshIndex);
    }

    void SceneMemoryManager::initializeAddTexture(Texture&& toAdd, const LogicalIndex textureIndex)
    {
        m_texturesMemory.add(std::move(toAdd), textureIndex);
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

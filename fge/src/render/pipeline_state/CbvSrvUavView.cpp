#include <fge/render/pipeline_state/CbvSrvUavView.hpp>

#include <fge/render/memory/SceneMemoryManager.hpp>
#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/Light.hpp>

#include <fge/resources/DescriptorHeapFactory.hpp>

namespace fge
{
    void CbvSrvUavView::initialize(ComPtr<ID3D12Device5> device, 
        SceneMemoryManager& sceneMemory)
    {
        // TO_DO Separer dans des fonctions distinctes

        const uint32_t nbDescriptors =
            1 +   // UAV output
            1 +   // TLAS
            3 + // light + material + indirectionMaterialTable
            sceneMemory.getNbMaxMeshes() + // vertex SRVs
            sceneMemory.getNbMaxSubMeshes() + // index SRVs
            sceneMemory.getNbMaxTextures();   // textures SRVs

        m_cbvSrvUavHeap = DescriptorHeapFactory::buildDescriptiorHeap(device, 
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, nbDescriptors,
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = 
            m_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = 
            m_cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
        
        m_outputTextureHostSrvHandle = cpuHandle;
        m_outputTextureSrvHandle = gpuHandle;
        sceneMemory.onCbsSrvUabViewInitializationCreateOutputUavTexture(device,
            m_outputTextureHostSrvHandle);
        
        m_handleIncrementationSize = device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        cpuHandle.ptr += m_handleIncrementationSize;
        gpuHandle.ptr += m_handleIncrementationSize;

        D3D12_SHADER_RESOURCE_VIEW_DESC tlasSrvDesc = {};
        tlasSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
        tlasSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        tlasSrvDesc.RaytracingAccelerationStructure.Location = 
            sceneMemory.getTlasBufferAddress();
        device->CreateShaderResourceView(nullptr, &tlasSrvDesc, cpuHandle);

        m_tlasSrvHandle = gpuHandle;

        cpuHandle.ptr += m_handleIncrementationSize;
        gpuHandle.ptr += m_handleIncrementationSize;

        // == Material SRV ==
        D3D12_SHADER_RESOURCE_VIEW_DESC materialSrvDesc = {};
        materialSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        materialSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        materialSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        materialSrvDesc.Buffer.FirstElement = 0;
        materialSrvDesc.Buffer.NumElements = sceneMemory.m_materialsMemory.getNbMaxMaterials();
        materialSrvDesc.Buffer.StructureByteStride = 
            sceneMemory.m_materialsMemory.getMaterialPageSize();
        materialSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(
            sceneMemory.getMaterialBufferResource(), &materialSrvDesc, cpuHandle);
        m_materialSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize;
        gpuHandle.ptr += m_handleIncrementationSize;

        // == Indirection Material Table SRV ==
        D3D12_SHADER_RESOURCE_VIEW_DESC indirectionMaterialTableSrvDesc = {};
        indirectionMaterialTableSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        indirectionMaterialTableSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        indirectionMaterialTableSrvDesc.Shader4ComponentMapping = 
            D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        indirectionMaterialTableSrvDesc.Buffer.FirstElement = 0;
        indirectionMaterialTableSrvDesc.Buffer.NumElements = 
            sceneMemory.m_materialsMemory.getNbMaxMaterials();
        indirectionMaterialTableSrvDesc.Buffer.StructureByteStride = sizeof(uint32_t);
        indirectionMaterialTableSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(
            sceneMemory.getIndirectionMaterialTableBufferResource(), 
            &indirectionMaterialTableSrvDesc, cpuHandle);
        m_indirectionMaterialTableSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize;
        gpuHandle.ptr += m_handleIncrementationSize;

        // == Light SRV ==
        D3D12_SHADER_RESOURCE_VIEW_DESC lightSrvDesc = {};
        lightSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        lightSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        lightSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        lightSrvDesc.Buffer.FirstElement = 0;
        lightSrvDesc.Buffer.NumElements = sceneMemory.m_lightsMemory.getNbMaxLights();
        lightSrvDesc.Buffer.StructureByteStride = sizeof(Light);
        lightSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(
            sceneMemory.getLightBufferResource(), &lightSrvDesc, cpuHandle);
        m_lightSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize;
        gpuHandle.ptr += m_handleIncrementationSize;
        
        m_baseVertexHostSrvHandle = cpuHandle;
        m_baseVertexSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxMeshes();
        gpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxMeshes();

        m_baseIndexHostSrvHandle = cpuHandle;
        m_baseIndexSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxSubMeshes();
        gpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxSubMeshes();

        sceneMemory.onCbsSrvUabViewInitializationFillMeshSrv(device, *this);
        
        m_baseTextureHostSrvHandle = cpuHandle;
        m_baseTextureSrvHandle = gpuHandle;
        cpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxTextures();
        gpuHandle.ptr += m_handleIncrementationSize * sceneMemory.getNbMaxTextures();
        
        D3D12_SHADER_RESOURCE_VIEW_DESC nullSrvDesc = {};
        nullSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        nullSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        nullSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        nullSrvDesc.Texture2D.MipLevels = 1;
        for(uint32_t i = 0; i < sceneMemory.getNbMaxTextures(); i++)
        {
            device->CreateShaderResourceView(nullptr, &nullSrvDesc, getTextureHostSrvHandle(i));
        }
        sceneMemory.onCbsSrvUabViewInitializationFillTextureSrv(device, *this);
    }

    void CbvSrvUavView::resize(ComPtr<ID3D12Device5> device, SceneMemoryManager& sceneMemory)
    {
        sceneMemory.onCbsSrvUabViewInitializationCreateOutputUavTexture(
            device, m_outputTextureHostSrvHandle);
    }

    ID3D12DescriptorHeap* CbvSrvUavView::getDescriptorHeap()
    {
        return m_cbvSrvUavHeap.Get();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getOutuputTextureSrvHandle()
    {
        return m_outputTextureSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getTlasSrvHandle()
    {
        return m_tlasSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getMaterialSrvHandle()
    {
        return m_materialSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getIndirectionMaterialTableSrvHandle()
    {
        return m_indirectionMaterialTableSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getLightSrvHandle()
    {
        return m_lightSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getBaseTextureSrvHandle()
    {
        return m_baseTextureSrvHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getVertexSrvHandle(const uint32_t meshIndex) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE result = m_baseVertexSrvHandle;
        result.ptr += (meshIndex * m_handleIncrementationSize);
        return result;
    }

    void CbvSrvUavView::setVertexSrvHandle(ComPtr<ID3D12Device5> device, 
        const uint32_t meshIndex, uint32_t nbVertices, ID3D12Resource* verticesBuffer)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC vertexSrvDesc = {};
        vertexSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        vertexSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        vertexSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        vertexSrvDesc.Buffer.FirstElement = 0;
        vertexSrvDesc.Buffer.NumElements = nbVertices;
        vertexSrvDesc.Buffer.StructureByteStride = sizeof(Vertex);
        vertexSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(
            verticesBuffer, &vertexSrvDesc, getVertexHostSrvHandle(meshIndex));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE CbvSrvUavView::getVertexHostSrvHandle(const uint32_t meshIndex)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE result = m_baseVertexHostSrvHandle;
        result.ptr += (meshIndex * m_handleIncrementationSize);
        return result;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getIndexSrvHandle(const uint32_t subMeshIndex) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE result = m_baseIndexSrvHandle;
        result.ptr += (subMeshIndex * m_handleIncrementationSize);
        return result;
    }

    void CbvSrvUavView::setIndexSrvHandle(ComPtr<ID3D12Device5> device, 
        const uint32_t subMeshIndex, uint32_t nbIndices, ID3D12Resource* indicesBuffer, 
        const uint32_t startIndex)
    {
        uint32_t firstIndiceIndex = startIndex;
        uint32_t nbTriangles = nbIndices / 3;

        D3D12_SHADER_RESOURCE_VIEW_DESC indexSrvDesc = {};
        indexSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        indexSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        indexSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        indexSrvDesc.Buffer.FirstElement = firstIndiceIndex / 3;
        throwIfFailed(firstIndiceIndex % 3 == 0, "Problem Algorithme");
        // TO_DO Changer les indices par un tableau de uint_32[3]
        indexSrvDesc.Buffer.NumElements = nbTriangles;
        indexSrvDesc.Buffer.StructureByteStride = sizeof(uint32_t) * 3;
        indexSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(
            indicesBuffer, &indexSrvDesc, getIndexHostSrvHandle(subMeshIndex));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE CbvSrvUavView::getIndexHostSrvHandle(const uint32_t subMeshIndex)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE result = m_baseIndexHostSrvHandle;
        result.ptr += (subMeshIndex * m_handleIncrementationSize);
        return result;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CbvSrvUavView::getTextureSrvHandle(const uint32_t textureIndex)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE result = m_baseTextureSrvHandle;
        result.ptr += (textureIndex * m_handleIncrementationSize);
        return result;
    }

    void CbvSrvUavView::setTextureSrvHandle(ComPtr<ID3D12Device5> device, 
        const uint32_t textureIndex, ID3D12Resource* textureResource)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC texSrvDesc = {};
        texSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        texSrvDesc.Format = textureResource->GetDesc().Format;
        texSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        texSrvDesc.Texture2D.MipLevels = textureResource->GetDesc().MipLevels;

        device->CreateShaderResourceView(textureResource, &texSrvDesc, 
            getTextureHostSrvHandle(textureIndex));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE CbvSrvUavView::getTextureHostSrvHandle(const uint32_t textureIndex)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE result = m_baseTextureHostSrvHandle;
        result.ptr += (textureIndex * m_handleIncrementationSize);
        return result;
    }
}
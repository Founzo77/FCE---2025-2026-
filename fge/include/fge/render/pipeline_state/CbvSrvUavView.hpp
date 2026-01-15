#pragma once

#include "../memory/UploadBuffer.hpp"

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>

using namespace Microsoft::WRL;

using std::vector;

namespace fge
{
    class SceneMemoryManager;

    class CbvSrvUavView
    {
    private:
        ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
        D3D12_GPU_DESCRIPTOR_HANDLE m_outputTextureSrvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE m_outputTextureHostSrvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_tlasSrvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_materialSrvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_indirectionMaterialTableSrvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_lightSrvHandle;

        D3D12_GPU_DESCRIPTOR_HANDLE m_baseVertexSrvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE m_baseVertexHostSrvHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_baseIndexSrvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE m_baseIndexHostSrvHandle;

        D3D12_GPU_DESCRIPTOR_HANDLE m_baseTextureSrvHandle;
        D3D12_CPU_DESCRIPTOR_HANDLE m_baseTextureHostSrvHandle;

        uint32_t m_handleIncrementationSize;

    public:
        CbvSrvUavView() = default;
        ~CbvSrvUavView() = default;

        void initialize(ComPtr<ID3D12Device5> device, SceneMemoryManager& sceneMemory);

        void resize(ComPtr<ID3D12Device5> device, SceneMemoryManager& sceneMemory);

        ID3D12DescriptorHeap* getDescriptorHeap();

        D3D12_GPU_DESCRIPTOR_HANDLE getOutuputTextureSrvHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE getTlasSrvHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE getMaterialSrvHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE getIndirectionMaterialTableSrvHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE getLightSrvHandle();
        D3D12_GPU_DESCRIPTOR_HANDLE getBaseTextureSrvHandle();

        D3D12_GPU_DESCRIPTOR_HANDLE getVertexSrvHandle(const uint32_t meshIndex) const;
        void setVertexSrvHandle(ComPtr<ID3D12Device5> device, const uint32_t meshIndex, 
            uint32_t nbVertices, ID3D12Resource* verticesBuffer);
    private:
        D3D12_CPU_DESCRIPTOR_HANDLE getVertexHostSrvHandle(const uint32_t meshIndex);

    public:
        D3D12_GPU_DESCRIPTOR_HANDLE getIndexSrvHandle(const uint32_t subMeshIndex) const;
        void setIndexSrvHandle(ComPtr<ID3D12Device5> device, const uint32_t subMeshIndex,
            uint32_t nbIndices, ID3D12Resource* indicesBuffer, const uint32_t startIndex);
    private:
        D3D12_CPU_DESCRIPTOR_HANDLE getIndexHostSrvHandle(const uint32_t subMeshIndex);

    public:
        D3D12_GPU_DESCRIPTOR_HANDLE getTextureSrvHandle(const uint32_t textureIndex);
        void setTextureSrvHandle(ComPtr<ID3D12Device5> device, const uint32_t textureIndex, 
            ID3D12Resource* textureResource);
    private:
        D3D12_CPU_DESCRIPTOR_HANDLE getTextureHostSrvHandle(const uint32_t textureIndex);
    };
}

#pragma once

#include "../memory/UploadBuffer.hpp"

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace fge
{
    class CbvSrvUavView;
    class SceneMemoryManager;

    class SbtTable
    {
    private:
        UploadBuffer m_sbtBuffer;
        void* m_rayGenShaderIdentifier;
        void* m_missShaderIdentifier;
        void* m_hitGroupIdentifier;

        uint32_t m_shaderIdentifierSize;
        uint32_t m_shaderTableSize;
        uint32_t m_hitGroupStride;
        uint32_t m_nbHitGroupTables;

    public:
        SbtTable() = default;
        ~SbtTable() = default;

        void initialize(ComPtr<ID3D12Device5> device, void* rayGenShaderIdentifier, 
            void* missShaderIdentifier, void* hitGroupIdentifier,
            const CbvSrvUavView& handleView, const SceneMemoryManager& sceneMemory);

        D3D12_GPU_VIRTUAL_ADDRESS_RANGE getRaygenerationShaderRecord();
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getMissShaderTable();
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getHitGroupTable();

        void startUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
    };
}

#pragma once

#include "../memory/UploadBuffer.hpp"

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>

using std::vector;

using namespace Microsoft::WRL;

namespace fge
{
    class CbvSrvUavView;
    class SceneMemoryManager;
    class SceneMemoryManager;
    class ShaderBank;

    class SbtTable
    {
    private:
        UploadBuffer m_sbtBuffer;

        uint32_t m_shaderIdentifierSize;
        uint32_t m_shaderTableSize;
        uint32_t m_hitGroupStride;
        uint32_t m_nbHitGroupTables;

    public:
        SbtTable() = default;
        ~SbtTable() = default;

        void initialize(ComPtr<ID3D12Device5> device, ShaderBank& shaderBank,
            const CbvSrvUavView& handleView, const SceneMemoryManager& sceneMemory);

        D3D12_GPU_VIRTUAL_ADDRESS_RANGE getRaygenerationShaderRecord();
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getMissShaderTable();
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE getHitGroupTable();

        void startUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
    };
}

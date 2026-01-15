#pragma once

#include "CbvSrvUavView.hpp"
#include "SbtTable.hpp"

#include <wrl/client.h>
#include <d3d12.h>
#include <memory>
#include <vector>

using namespace Microsoft::WRL;
using std::shared_ptr;
using std::vector;

namespace fge
{
    class SceneMemoryManager;

    constexpr uint32_t SHADER_REGISTER_CONSTANT_INFORMATION = 0;
    constexpr uint32_t SHADER_REGISTER_HIT_LOCAL = 1;
    
    constexpr uint32_t SHADER_REGISTER_OUTPUT = 0;
    
    constexpr uint32_t SHADER_REGISTER_SCENE = 0;
    constexpr uint32_t SHADER_REGISTER_VERTICES = 1;
    constexpr uint32_t SHADER_REGISTER_INDICES = 2;
    constexpr uint32_t SHADER_REGISTER_MATERIALS = 3;
    constexpr uint32_t SHADER_REGISTER_INDIRECTION_MATERIAL_TABLE = 4;
    constexpr uint32_t SHADER_REGISTER_LIGHTS = 5;
    constexpr uint32_t SHADER_REGISTER_TEXTURES = 6;

    constexpr uint32_t SHADER_REGISTER_SAMPLER = 0;

    class RayTracingPipeline
    {
    private:
        uint32_t m_width;
        uint32_t m_height;
    private:
        CbvSrvUavView m_handleView;
        SbtTable m_sbtTable;

        ComPtr<ID3D12RootSignature> m_globalRootSignature;
        ComPtr<ID3D12RootSignature> m_localRootSignature;
        ComPtr<ID3D12StateObject> m_pipelineState;
        ComPtr<ID3D12StateObjectProperties> m_pipelineProperties;

        shared_ptr<SceneMemoryManager> m_sceneMemory;

    public:
        RayTracingPipeline() = default;
        ~RayTracingPipeline() = default;

        void initialize(ComPtr<ID3D12Device5> device, 
            shared_ptr<SceneMemoryManager> sceneMemory, 
            uint32_t width, uint32_t height);

        void resize(ComPtr<ID3D12Device5> device, uint32_t width, uint32_t height);
        
        void dispatchRays(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);

    private:
        void initializecbvSrvUavResources(ComPtr<ID3D12Device5> device);
        void initializeRootSignature(ComPtr<ID3D12Device5> device);
        void initializePipelineState(ComPtr<ID3D12Device5> device);
        void initializeSBT(ComPtr<ID3D12Device5> device);
    };
}

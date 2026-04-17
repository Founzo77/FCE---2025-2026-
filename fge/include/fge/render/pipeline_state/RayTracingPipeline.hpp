#pragma once

#include "CbvSrvUavView.hpp"
#include "SbtTable.hpp"
#include "ShaderBank.hpp"

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

    constexpr uint32_t SHADER_SPACE_SCENE_DATA = 0;
    constexpr uint32_t SHADER_SPACE_SURFACIQUE_DATA = 1;
    constexpr uint32_t SHADER_SPACE_VOLUMIQUE_DATA = 2;

    constexpr uint32_t SHADER_REGISTER_CONSTANT_INFORMATION = 0; // b0 space0
    constexpr uint32_t SHADER_REGISTER_HIT_LOCAL = 0; // b0 space1
    
    constexpr uint32_t SHADER_REGISTER_OUTPUT = 0; // u0 space0
    constexpr uint32_t SHADER_REGISTER_ACCUMULATED_INPUT = 1; // u1 space0
    constexpr uint32_t SHADER_REGISTER_ACCUMULATED_OUTPUT = 2; // u2 space0
    
    constexpr uint32_t SHADER_REGISTER_SCENE_TLAS = 0; // t0 space0
    constexpr uint32_t SHADER_REGISTER_LIGHTS = 1; // t1 space0
    constexpr uint32_t SHADER_REGISTER_VERTICES = 0; // t0 space1
    constexpr uint32_t SHADER_REGISTER_INDICES = 1; // t1 space1
    constexpr uint32_t SHADER_REGISTER_MATERIALS = 2; // t2 space1
    constexpr uint32_t SHADER_REGISTER_INDIRECTION_MATERIAL_TABLE = 3; // t3 space1
    constexpr uint32_t SHADER_REGISTER_TEXTURES = 4; // t4 space1

    constexpr uint32_t SHADER_REGISTER_TEXTURES_3D = 2; // t2 space2

    constexpr uint32_t SHADER_REGISTER_SAMPLER = 0; // s0 space0

    class RayTracingPipeline
    {
    private:
        uint32_t m_width;
        uint32_t m_height;
    private:
        CbvSrvUavView m_handleView;
        CbvSrvUavView m_handleHostView;
        SbtTable m_sbtTable;
        ShaderBank m_shaderBank;

        ComPtr<ID3D12Device5> m_device;
        ComPtr<ID3D12RootSignature> m_globalRootSignature;
        ComPtr<ID3D12RootSignature> m_localRootSurfaceSignature;
        ComPtr<ID3D12RootSignature> m_localRootVolumeSignature;
        ComPtr<ID3D12StateObject> m_pipelineState;
        ComPtr<ID3D12StateObjectProperties> m_pipelineProperties;

        shared_ptr<SceneMemoryManager> m_sceneMemory;

    public:
        RayTracingPipeline() = default;
        ~RayTracingPipeline();

        void initialize(ComPtr<ID3D12Device5> device, 
            shared_ptr<SceneMemoryManager> sceneMemory, uint32_t width, uint32_t height);

        void reset();
    
        CbvSrvUavView& getHandleView();
        CbvSrvUavView& getHandleHostView();

        uint32_t getWidth() const noexcept;
        uint32_t getHeight() const noexcept;

        void resize(uint32_t width, uint32_t height);
        
        void bindDescriptors(ComPtr<ID3D12GraphicsCommandList4> directCommandList,
            const uint32_t cumputeFrameIndex);
        void dispatchRays(ComPtr<ID3D12GraphicsCommandList4> directCommandList);

    private:
        void initializecbvSrvUavResources();
        void initializeRootSignature();
        void initializePipelineState();
        void initializeSBT();
    };
}

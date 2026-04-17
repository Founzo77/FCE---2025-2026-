#pragma once

#include "CommandQueue.hpp"
#include "pipeline_state/RayTracingPipeline.hpp"
#include "Scene.hpp"
#include "FrameContext.hpp"
#include "FrameExecutionContext.hpp"

#include "../io/SceneDescription.hpp"

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>

using namespace Microsoft::WRL;
using std::shared_ptr;
using std::vector;

namespace fge
{
    class SceneDescription;

    class RenderEngine
    {
    private:
        ComPtr<ID3D12Device5> m_device;
        shared_ptr<Scene> m_scene;
        shared_ptr<RayTracingPipeline> m_rayTracingPipeline;
        CommandQueue m_directCommandQueue;
        vector<FrameContext> m_frameContexts;
        uint32_t m_currentIdFrame;
        ComPtr<ID3D12Fence> m_renderFence;
        HANDLE m_renderFenceEvent;
        uint64_t m_renderFenceValue;

    public:
        RenderEngine() = default;
        ~RenderEngine();

        void initialize(ComPtr<ID3D12Device5> device, const uint32_t width,
            const uint32_t height, SceneDescription& sceneDescription,
            const uint32_t nbFrameContexts);
            
    private:
        void initializeDeviceResources(const uint32_t width,
            const uint32_t height, const uint32_t nbFrameContexts);
        void initializeScene(SceneDescription& sceneDescription);

    public:
        void reset();
        
        shared_ptr<Scene> getScene();
        ID3D12Resource* getOutputTexture();
        HANDLE getSharedOutputTextureHandle();
        ID3D12CommandQueue* getDirectCommandQueue();

        void resize(const uint32_t width, const uint32_t height);
        FrameExecutionContext startRender();
        void executeRender();
        void copyOutputToHost();
        void copyOutputToSharedResource();
        void endRender();
        void synchronize();
        void copyOutputHostToHost(void* dstBuffer, uint64_t dstSize);
    };
}
#pragma once

#include "FrameContext.hpp"
#include "CommandQueue.hpp"
#include "pipeline_state/RayTracingPipeline.hpp"
#include "WriterFrame.hpp"

#include "Scene.hpp"

#include <fge/io/SceneDescription.hpp>

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#include <d3d11.h>
#include <d3d11on12.h>
#include <d2d1_1.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

using namespace Microsoft::WRL;
using std::shared_ptr;
using std::string;
using std::vector;

namespace fge
{
    class DeviceMemorySceneManager;
    class SceneDescription;

    class MainRenderer
    {
    private:
        ComPtr<ID3D12Device5> m_device;
        ComPtr<ID3D11Device> m_11Device;
        ComPtr<ID3D11DeviceContext> m_11DeviceContext;
        ComPtr<ID3D11On12Device2> m_11On12Device;
        ComPtr<IDXGIDevice> m_dxgiDevice;
        ComPtr<ID2D1Device> m_2DDevice;
        ComPtr<ID2D1DeviceContext> m_2DDeviceContext;

        ComPtr<IDXGISwapChain4> m_swapChain;
        vector<FrameContext> m_frames;
        ComPtr<ID3D12Fence> m_renderFence;
        HANDLE m_renderEvent;
        uint64_t m_renderValue;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        CommandQueue m_directCommandQueue;
        UINT m_currentIdBackBuffer;
        shared_ptr<Scene> m_scene;
        shared_ptr<RayTracingPipeline> m_rayTracingPipeline;
        WriterFrame m_writerFrame;

    public:
        MainRenderer() = default;
        ~MainRenderer();

        void initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
            uint32_t width, uint32_t height, SceneDescription& sceneDescription, 
            uint32_t nbFrames);
        void resize(uint32_t width, uint32_t height);
        void renderFrame();

        inline shared_ptr<Scene> getScene() noexcept
            { return m_scene; }

    private:
        void initializeDeviceResources(HWND hWnd, ComPtr<ID3D12Device5> device,
            uint32_t width, uint32_t height, uint32_t nbFrames);
        void initializeScene(SceneDescription& sceneDescription);

        void updateRenderTargetViews();
        void clearFrame();
        void updateFrame();
    };
}

#pragma once

#include "PresentFrame.hpp"

#include <fge/render/FrameContext.hpp>
#include <fge/render/CommandQueue.hpp>
#include <fge/PlatformWindows.hpp>
#include <fge/io/SceneDescription.hpp>
#include <fge/render/SwapChainPresenter.hpp>
#include <fge/render/FrameComposer.hpp>
#include <fge/render/data/BarrierBatch.hpp>

#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#include <memory>
#include <vector>

using namespace Microsoft::WRL;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

namespace fgewa
{
    class AnariScene;
    class Device;
    class ImplementationCapability;

    class MainRenderer
    {
    private:
        shared_ptr<AnariScene> m_scene;
        uint32_t m_width;
        uint32_t m_height;
        shared_ptr<Device> m_device;
        shared_ptr<ImplementationCapability> m_capability;
        bool m_isFirstFrame;

        ComPtr<ID3D12Device5> m_dxrDevice;
        fge::CommandQueue m_directCommandQueue;
        unique_ptr<PresentFrame> m_presentFrame;
        fge::FrameComposer m_frameComposer;
        fge::BarrierBatch m_barriers;
        vector<fge::FrameContext> m_frames;
        ComPtr<ID3D12Fence> m_renderFence;
        HANDLE m_renderEvent;
        uint64_t m_renderValue;
        uint32_t m_currentIdFrame;

    public:
        MainRenderer() = default;

        void initialize(shared_ptr<Device> device, 
            shared_ptr<ImplementationCapability> capability, const uint32_t width,
            const uint32_t height, fge::SceneDescription& sceneDescription,
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, uint32_t nbFrames);
    
        shared_ptr<AnariScene> getScene();
        void renderFrame();
        void resize(uint32_t width, uint32_t height);

    private:
        void initializeDeviceResources(HWND hWnd, ComPtr<ID3D12Device5> device,
            uint32_t width, uint32_t height, uint32_t nbFrames);

        void computeAnariFrame();
    };
}
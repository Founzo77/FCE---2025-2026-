#pragma once

#include "SwapChainPresenter.hpp"
#include "FrameComposer.hpp"
#include "RenderEngine.hpp"
#include "Scene.hpp"
#include "data/BarrierBatch.hpp"

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;
using std::shared_ptr;

namespace fge
{
    class DeviceMemorySceneManager;
    class SceneDescription;

    class MainRenderer
    {
    private:
        ComPtr<ID3D12Device5> m_device;
        SwapChainPresenter m_swapChainPresenter;
        FrameComposer m_frameComposer;
        RenderEngine m_renderEngine;
        BarrierBatch m_barriers;

    public:
        MainRenderer() = default;
        ~MainRenderer();

        void initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
            uint32_t width, uint32_t height, SceneDescription& sceneDescription, 
            uint32_t nbFrames);
        void reset();

        shared_ptr<Scene> getScene() noexcept;

        void resize(uint32_t width, uint32_t height);
        void renderFrame();
    };
}

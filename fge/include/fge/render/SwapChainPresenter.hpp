#pragma once

#include "FramePresentationContext.hpp"

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class FrameExecutionContext;
    class BarrierBatch;

    class SwapChainPresenter
    {
    protected:
        ComPtr<ID3D12Device5> m_device;
        ComPtr<IDXGISwapChain4> m_swapChain;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        vector<FramePresentationContext> m_framePresentationContexts;
        UINT m_currentIdBackBuffer;

    public:
        SwapChainPresenter() = default;
        virtual ~SwapChainPresenter();

        void initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
            ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers);
        virtual void reset();
        void resize(const uint32_t width, const uint32_t height);

    private:
        void updateRenderTargetViews();
    
    public:
        void copyOutputToRTV(FrameExecutionContext& context);
        void presentFrame();
        void setBackBufferState(BarrierBatch& barrierBatch,
            const D3D12_RESOURCE_STATES beforState, const D3D12_RESOURCE_STATES afterState);
        void setOmSetRenderTarget(FrameExecutionContext& context);
    };
}
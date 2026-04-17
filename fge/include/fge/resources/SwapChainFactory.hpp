#pragma once

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class SwapChainFactory
    {
    public:
        static ComPtr<IDXGISwapChain4> buildSwapChainV4(HWND hWnd,
            ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height,
            uint32_t nbFrame);
        static ComPtr<IDXGISwapChain4> buildSwapChainV4(HWND hWnd,
            ID3D12CommandQueue* commandQueue, uint32_t width, uint32_t height,
            uint32_t nbFrame);
    };
}

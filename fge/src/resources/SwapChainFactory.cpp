#include <fge/resources/SwapChainFactory.hpp>
#include <fge/utility.hpp>
#include <fge/resources/utility.hpp>

namespace fge
{
    ComPtr<IDXGISwapChain4> SwapChainFactory::buildSwapChainV4(HWND hWnd, 
        ComPtr<ID3D12CommandQueue> commandQueue, uint32_t width, uint32_t height, 
        uint32_t nbFrame)
    {
        ComPtr<IDXGISwapChain4> swapChain4;
        ComPtr<IDXGIFactory4> factory4;

        UINT factoryFlags = 0;
        #if defined(_DEBUG)
            factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        #endif

        throwIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory4)),
            "Failed to create IDXGIFactory4");

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = nbFrame;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = checkTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        
        ComPtr<IDXGISwapChain1> swapChain1;
        throwIfFailed(factory4->CreateSwapChainForHwnd(commandQueue.Get(), hWnd,
            &swapChainDesc, nullptr, nullptr, &swapChain1),
            "Failed to create IDXGISwapChain1");

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        throwIfFailed(factory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
        throwIfFailed(swapChain1.As(&swapChain4), 
            "Failed to convert IDXGISwapChain1 to IDXGISwapChain4");

        return swapChain4;
    }
}

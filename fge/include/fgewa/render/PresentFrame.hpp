#pragma once

#include <fge/render/SwapChainPresenter.hpp>

#include <fge/PlatformWindows.hpp>
#include <wrl/client.h>
#include <d3d12.h>

#include <memory>

using namespace Microsoft::WRL;
using std::shared_ptr;

namespace fge
{
    class BarrierBatch;
}

namespace fgewa
{
    class Device;
    class Frame;

    class PresentFrame : public fge::SwapChainPresenter
    {
    protected:
        shared_ptr<Device> m_device = nullptr;
        ComPtr<ID3D12Device5> m_dxrDevice;
        D3D12_RESOURCE_DESC m_bufferTargetDescription;

    public:
        PresentFrame() = default;
        virtual ~PresentFrame() = default;

        PresentFrame(const PresentFrame&) = default;
        PresentFrame& operator=(const PresentFrame&) = default;
        PresentFrame(PresentFrame&& other) = default;
        PresentFrame& operator=(PresentFrame&& other) = default;

        virtual void initialize(shared_ptr<Device> device, 
            HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
            const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers);
        virtual void resize(const uint32_t width, const uint32_t height);
        virtual void copyOutputToRTV(ID3D12GraphicsCommandList4* commandList,
            Frame& frame) = 0;
    };
}
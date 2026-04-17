#include <fgewa/render/PresentFrame.hpp>

namespace fgewa
{
    void PresentFrame::initialize(shared_ptr<Device> device, 
        HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, ID3D12CommandQueue* commandQueue,
        const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers)
    {
        m_device = device;
        m_dxrDevice = dxrDevice;
        fge::SwapChainPresenter::initialize(
            hWnd, dxrDevice, commandQueue, width, height, nbBackBuffers);
        m_bufferTargetDescription = 
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer->GetDesc();
    }

    void PresentFrame::resize(const uint32_t width, const uint32_t height)
    {
        fge::SwapChainPresenter::resize(width, height);
    }
}

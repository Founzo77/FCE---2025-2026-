#include <fge/render/FrameComposer.hpp>
#include <fge/render/FrameExecutionContext.hpp>

#include <d3dx12.h>

namespace fge
{
    FrameComposer::~FrameComposer()
    {
        reset();
    }

    void FrameComposer::initialize(HWND hWnd, ComPtr<ID3D12Device5> device, 
        const uint32_t nbBackBuffers, const uint32_t width, const uint32_t height)
    {
        m_imGuiLayer.initialize(hWnd, device, nbBackBuffers, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        m_width = width;
        m_height = height;
    }

    void FrameComposer::reset()
    {
        m_imGuiLayer.reset();
        m_device.Reset();
        m_width = 0;
        m_height = 0;
    }

    void FrameComposer::resize(const uint32_t width, const uint32_t height)
    {
        // TO_DO
        m_width = width;
        m_height = height;
    }

    void FrameComposer::renderFrame(FrameExecutionContext& context)
    {
        D3D12_VIEWPORT viewport{};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(m_width);
        viewport.Height = static_cast<float>(m_height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        D3D12_RECT scissor{};
        scissor.left = 0;
        scissor.top = 0;
        scissor.right = static_cast<LONG>(m_width);
        scissor.bottom = static_cast<LONG>(m_height);

        context.m_commandList->RSSetViewports(1, &viewport);
        context.m_commandList->RSSetScissorRects(1, &scissor);

        m_imGuiLayer.beginFrame();
        m_imGuiLayer.drawUI();
        m_imGuiLayer.render(context.m_commandList);
    }
}
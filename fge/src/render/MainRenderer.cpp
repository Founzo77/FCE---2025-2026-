#include <fge/render/MainRenderer.hpp>
#include <fge/render/Scene.hpp>

#include <fge/resources/SwapChainFactory.hpp>
#include <fge/resources/FenceFactory.hpp>
#include <fge/resources/EventFactory.hpp>
#include <fge/resources/DescriptorHeapFactory.hpp>

#include <fge/utility.hpp>
#include <fge/render/utility.hpp>

namespace fge
{
    MainRenderer::~MainRenderer()
    {
        reset();
    }

    void MainRenderer::initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
        uint32_t width, uint32_t height, SceneDescription& sceneDescription, uint32_t nbFrames)
    {
        m_renderEngine.initialize(device, width, height, sceneDescription, 2);
        m_frameComposer.initialize(hWnd, device, nbFrames, width, height);
        m_swapChainPresenter.initialize(hWnd, device, 
            m_renderEngine.getDirectCommandQueue(), width, height, nbFrames);
    }

    void MainRenderer::reset()
    {
        m_barriers.reset();
        m_swapChainPresenter.reset();
        m_frameComposer.reset();
        m_renderEngine.reset();
        m_device.Reset();
    }

    shared_ptr<Scene> MainRenderer::getScene() noexcept
    {
        return m_renderEngine.getScene();
    }

    void MainRenderer::resize(uint32_t width, uint32_t height)
    {
        m_renderEngine.resize(width, height);
        m_frameComposer.resize(width, height);
        m_swapChainPresenter.resize(width, height);
    }

    void MainRenderer::renderFrame()
    {
        // TO_DO Avoir plusieurs queues et Buffer pour pouvoir commencer le RT quand 
        // on fait la rasterization ImGui

        FrameExecutionContext executionContext = m_renderEngine.startRender();

        executionContext.setOutputTextureState(m_barriers,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
        m_swapChainPresenter.setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
        m_barriers.submit(executionContext.m_commandList);
        
        m_swapChainPresenter.copyOutputToRTV(executionContext);

        executionContext.setOutputTextureState(m_barriers,
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        m_swapChainPresenter.setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_barriers.submit(executionContext.m_commandList);

        m_swapChainPresenter.setOmSetRenderTarget(executionContext);
        m_frameComposer.renderFrame(executionContext);

        m_swapChainPresenter.setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_barriers.submit(executionContext.m_commandList);

        m_renderEngine.executeRender();
        m_swapChainPresenter.presentFrame();
        m_renderEngine.endRender();
    }
}

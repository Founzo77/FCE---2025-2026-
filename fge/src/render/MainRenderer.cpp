#include <fge/render/MainRenderer.hpp>
#include <fge/render/Scene.hpp>

#include <fge/resources/SwapChainFactory.hpp>
#include <fge/resources/FenceFactory.hpp>
#include <fge/resources/EventFactory.hpp>
#include <fge/resources/DescriptorHeapFactory.hpp>
#include <fge/resources/DirectXDeviceFactory.hpp>

#include <fge/utility.hpp>
#include <fge/render/utility.hpp>

namespace fge
{
    fge::MainRenderer::~MainRenderer()
    {
        m_directCommandQueue.flush(m_renderFence, m_renderEvent, m_renderValue);
        CloseHandle(m_renderEvent);
    }

    void MainRenderer::initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
        uint32_t width, uint32_t height, SceneDescription& sceneDescription, uint32_t nbFrames)
    {
        initializeDeviceResources(hWnd, device, width, height, nbFrames);
        //m_writerFrame.initialize(m_11On12Device, m_2DDeviceContext, m_frames);
        initializeScene(sceneDescription);

        throwIfFailed(m_frames[m_currentIdBackBuffer].m_directCommandList->Close(),
            "Failed to close m_directCommandList");

        ID3D12CommandList* const commandLists[] = {
            m_frames[m_currentIdBackBuffer].m_directCommandList.Get()
        };
        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);

        signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderValue);
        m_frames[m_currentIdBackBuffer].m_fenceValue = m_renderValue;

        // TO_DO Regarder si synchro peut se faire apres m_rayTracingPipeline.initialize(...)
        waitForFenceValue(m_renderFence, m_renderEvent, 
            m_frames[m_currentIdBackBuffer].m_fenceValue);
        
        m_rayTracingPipeline = std::make_shared<RayTracingPipeline>();
        m_rayTracingPipeline->initialize(device, m_scene->getMemory(), width, height);
        m_scene->getMemory()->setRayTracingPipeline(m_rayTracingPipeline);
    }

    void MainRenderer::resize(uint32_t width, uint32_t height)
    {
        m_directCommandQueue.flush(m_renderFence, m_renderEvent, m_renderValue);

        for(size_t i = 0; i < m_frames.size(); i++)
        {
            m_frames[i].resetBackBuffer(m_frames[m_currentIdBackBuffer].getFenceValue());
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        throwIfFailed(m_swapChain->GetDesc(&swapChainDesc));
        throwIfFailed(m_swapChain->ResizeBuffers(m_frames.size(), width, height,
            swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        updateRenderTargetViews();
        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

        m_rayTracingPipeline->resize(m_device, width, height);
    }

    void MainRenderer::renderFrame()
    {
        m_frames[m_currentIdBackBuffer].resetCommand();

        m_scene->getMemory()->updateDevice(
            m_device, m_frames[m_currentIdBackBuffer].m_directCommandList);

        updateFrame();
        
        throwIfFailed(m_frames[m_currentIdBackBuffer].m_directCommandList->Close());

        ID3D12CommandList* const commandLists[] = {
            m_frames[m_currentIdBackBuffer].m_directCommandList.Get()
        };

        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);

        //m_writerFrame.writeToTexture("Test", m_currentIdBackBuffer);
        //m_11DeviceContext->Flush();

        // TO_DO VSync 
        // TO_DO Tearing support & VSync
        //UINT syncInterval = 0; UINT presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        UINT syncInterval = 1; UINT presentFlags = 0;
        throwIfFailed(m_swapChain->Present(syncInterval, presentFlags));
        
        signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderValue);
        m_frames[m_currentIdBackBuffer].m_fenceValue = m_renderValue;

        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

        waitForFenceValue(m_renderFence, m_renderEvent, 
            m_frames[m_currentIdBackBuffer].m_fenceValue);
    }

    void MainRenderer::initializeDeviceResources(HWND hWnd, ComPtr<ID3D12Device5> device,
        uint32_t width, uint32_t height, uint32_t nbFrames)
    {
        m_device = device;

        m_directCommandQueue.initialize(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_swapChain = SwapChainFactory::buildSwapChainV4(hWnd, 
            m_directCommandQueue.m_commandQueue, width, height, nbFrames);

        m_renderFence = FenceFactory::buildFence(m_device);
        m_renderEvent = EventFactory::buildEvent();
        m_renderValue = 0;

        m_rtvHeap = DescriptorHeapFactory::buildDescriptiorHeap(m_device,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, nbFrames);
        m_frames.resize(nbFrames);
        
        UINT rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for(size_t i = 0; i < m_frames.size(); i++)
        {
            ComPtr<ID3D12Resource> backBuffer;
            throwIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
            m_frames[i].initialize(m_device, rtvDescriptorSize, rtvHandle, backBuffer);
        }

        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

        m_frames[m_currentIdBackBuffer].resetCommand();

        // TO_DO Mettre ca dans un objet contexte 2D ou directx11
        /*
        DirectXDeviceFactory::buildDirectX11On12Device(m_device,
            m_directCommandQueue.m_commandQueue, &m_11Device, &m_11DeviceContext, &m_11On12Device);
        m_dxgiDevice = DirectXDeviceFactory::buildDxgiDevice(m_11On12Device);
        m_2DDevice = DirectXDeviceFactory::build2DDevice(m_dxgiDevice);
        m_2DDeviceContext = DirectXDeviceFactory::buildDirectX2DDeviceContext(m_2DDevice);
        */
    }

    void MainRenderer::initializeScene(SceneDescription& sceneDescription)
    {
        m_scene = std::make_shared<Scene>();
        m_scene->buildScene(
            m_device, m_frames[m_currentIdBackBuffer].m_directCommandList, sceneDescription);
    }

    void MainRenderer::updateRenderTargetViews()
    {
        UINT rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for(size_t i = 0; i < m_frames.size(); i++)
        {
            ComPtr<ID3D12Resource> backBuffer;
            throwIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
            m_frames[i].updateRenderTargetView(
                m_device, rtvDescriptorSize, rtvHandle, backBuffer);
        }
    }

    void MainRenderer::clearFrame()
    {
        FLOAT clearColor[] = { 0.4f, 0.2f, 0.9, 1.0f };
        m_frames[m_currentIdBackBuffer].m_directCommandList->ClearRenderTargetView(
            m_frames[m_currentIdBackBuffer].m_rtvHandle, clearColor, 0, nullptr);
    }

    void MainRenderer::updateFrame()
    {
        m_rayTracingPipeline->dispatchRays(m_device, 
            m_frames[m_currentIdBackBuffer].m_directCommandList);
        
        // TO_DO Changer le rander target de la ray tracing pipeline par 
        // m_frames[m_currentIdBackBuffer].m_backBuffer
        CD3DX12_RESOURCE_BARRIER barriers[2] = {
            CD3DX12_RESOURCE_BARRIER::Transition(
                m_scene->getMemory()->getOutputTextureBufferResource(),
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(
                m_frames[m_currentIdBackBuffer].m_backBuffer.Get(),
                D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST) 
        };

        m_frames[m_currentIdBackBuffer].m_directCommandList->ResourceBarrier(2, barriers);
        m_frames[m_currentIdBackBuffer].m_directCommandList->CopyResource(
            m_frames[m_currentIdBackBuffer].m_backBuffer.Get(), 
            m_scene->getMemory()->getOutputTextureBufferResource());
        
        /*
        barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
                m_frames[m_currentIdBackBuffer].m_backBuffer.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_frames[m_currentIdBackBuffer].m_directCommandList->ResourceBarrier(1, barriers);
        */
        
        barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_scene->getMemory()->getOutputTextureBufferResource(),
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_frames[m_currentIdBackBuffer].m_backBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);

        m_frames[m_currentIdBackBuffer].m_directCommandList->ResourceBarrier(2, barriers);
    }
}

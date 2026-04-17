#include <fge/render/SwapChainPresenter.hpp>
#include <fge/render/FrameExecutionContext.hpp>

#include <fge/render/data/BarrierBatch.hpp>

#include <fge/resources/SwapChainFactory.hpp>
#include <fge/resources/DescriptorHeapFactory.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

using fge::throwIfFailed;

namespace fge
{
    SwapChainPresenter::~SwapChainPresenter()
    {
        reset();
    }

    void SwapChainPresenter::initialize(HWND hWnd, ComPtr<ID3D12Device5> device, 
        ID3D12CommandQueue* commandQueue,
        const uint32_t width, const uint32_t height, const uint32_t nbBackBuffers)
    {
        m_device = device;

        m_swapChain = SwapChainFactory::buildSwapChainV4(hWnd, 
            commandQueue, width, height, nbBackBuffers);
        d12SetDebugName(m_swapChain, L"SwapChain Windows");

        m_rtvHeap = DescriptorHeapFactory::buildDescriptiorHeap(m_device,
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, nbBackBuffers);
        d12SetDebugName(m_rtvHeap, L"RTV Windows");

        UINT rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        m_framePresentationContexts.resize(nbBackBuffers);

        for(size_t i = 0; i < m_framePresentationContexts.size(); i++)
        {
            ComPtr<ID3D12Resource> backBuffer;
            throwIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
            m_framePresentationContexts[i].initialize(
                m_device, rtvDescriptorSize, rtvHandle, backBuffer);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }

        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChainPresenter::reset()
    {
        for(FramePresentationContext& context : m_framePresentationContexts)
            context.reset();
        m_framePresentationContexts.clear();
        m_framePresentationContexts.shrink_to_fit();
        m_currentIdBackBuffer = 0;
        
        m_rtvHeap.Reset();
        m_swapChain.Reset();
        m_device.Reset();
    }

    void SwapChainPresenter::resize(const uint32_t width, const uint32_t height)
    {
        for(size_t i = 0; i < m_framePresentationContexts.size(); i++)
        {
            m_framePresentationContexts[i].resetBackBuffer();
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        throwIfFailed(m_swapChain->GetDesc(&swapChainDesc));
        throwIfFailed(m_swapChain->ResizeBuffers(
            m_framePresentationContexts.size(), width, height,
            swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

        updateRenderTargetViews();
        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChainPresenter::updateRenderTargetViews()
    {
        UINT rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
            m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for(size_t i = 0; i < m_framePresentationContexts.size(); i++)
        {
            ComPtr<ID3D12Resource> backBuffer;
            throwIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
            m_framePresentationContexts[i].updateRenderTargetView(
                m_device, rtvDescriptorSize, rtvHandle, backBuffer);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    void SwapChainPresenter::copyOutputToRTV(FrameExecutionContext& context)
    {
        context.m_commandList->CopyResource(
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer.Get(), 
            context.m_outputTexture);
    }

    void SwapChainPresenter::presentFrame()
    {
        // TO_DO Tearing support & VSync
        UINT syncInterval = 0; UINT presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        //UINT syncInterval = 1; UINT presentFlags = 0;
        throwIfFailed(m_swapChain->Present(syncInterval, presentFlags));
        m_currentIdBackBuffer = m_swapChain->GetCurrentBackBufferIndex();
    }

    void SwapChainPresenter::setBackBufferState(BarrierBatch& barrierBatch,
        const D3D12_RESOURCE_STATES beforState, const D3D12_RESOURCE_STATES afterState)
    {
        barrierBatch.add(CD3DX12_RESOURCE_BARRIER::Transition(
            m_framePresentationContexts[m_currentIdBackBuffer].m_backBuffer.Get(),
            beforState, afterState));
    }

    void SwapChainPresenter::setOmSetRenderTarget(FrameExecutionContext& context)
    {
        context.m_commandList->OMSetRenderTargets(1, 
            &m_framePresentationContexts[m_currentIdBackBuffer].m_rtvHandle, FALSE, nullptr);
    }
}
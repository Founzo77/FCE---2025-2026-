#include <fge/render/FrameContext.hpp>
#include <fge/resources/CommandAllocatorFactory.hpp>
#include <fge/resources/CommandListFactory.hpp>

namespace fge
{
    void FrameContext::initialize(ComPtr<ID3D12Device5> device, UINT rtvDescriptorSize,
            CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, ComPtr<ID3D12Resource> backBuffer)
    {
        m_directCommandAllocator = CommandAllocatorFactory::buildCommandAllocator(device,
            D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_directCommandList = CommandListFactory::buildCommandListV4(device,
            m_directCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_fenceValue = 0;

        updateRenderTargetView(device, rtvDescriptorSize, rtvHandle, backBuffer);
        //resetCommand();
    }

    void FrameContext::updateRenderTargetView(ComPtr<ID3D12Device5> device, 
        UINT rtvDescriptorSize, CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, 
        ComPtr<ID3D12Resource> backBuffer)
    {
        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
        m_backBuffer = backBuffer;
        m_rtvHandle = rtvHandle;
        rtvHandle.Offset(rtvDescriptorSize);
    }

    void FrameContext::resetBackBuffer(uint64_t fenceValue)
    {
        m_backBuffer.Reset();
        m_fenceValue = fenceValue;
    }

    void FrameContext::resetCommand()
    {
        m_directCommandAllocator->Reset();
        m_directCommandList->Reset(m_directCommandAllocator.Get(), nullptr);
    }

    void FrameContext::setPresentState()
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        m_directCommandList->ResourceBarrier(1, &barrier);
    }

    void FrameContext::setRenderTargetState()
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_directCommandList->ResourceBarrier(1, &barrier);
    }
}
#include <fge/render/FramePresentationContext.hpp>

#include <fge/utility.hpp>

namespace fge
{
    FramePresentationContext::~FramePresentationContext()
    {
        reset();
    }

    void FramePresentationContext::initialize(ComPtr<ID3D12Device5> device, 
        const UINT rtvDescriptorSize, CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle, 
        ComPtr<ID3D12Resource> backBuffer)
    {
        updateRenderTargetView(device, rtvDescriptorSize, rtvHandle, backBuffer);
    }

    void FramePresentationContext::reset()
    {
        m_rtvHandle = {};
        m_backBuffer.Reset();
    }

    void FramePresentationContext::updateRenderTargetView(ComPtr<ID3D12Device5> device, 
        const UINT rtvDescriptorSize, CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle, 
        ComPtr<ID3D12Resource> backBuffer)
    {
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        rtvDesc.Texture2D.PlaneSlice = 0;

        device->CreateRenderTargetView(backBuffer.Get(), &rtvDesc, rtvHandle);
        d12SetDebugName(backBuffer, L"BackBuffer Windows");
        m_backBuffer = backBuffer;
        m_rtvHandle = rtvHandle;
        rtvHandle.Offset(rtvDescriptorSize);
    }

    void FramePresentationContext::resetBackBuffer()
    {
        m_backBuffer.Reset();
    }

    void FramePresentationContext::setPresentState(
        ID3D12GraphicsCommandList4* directCommandList)
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);
        directCommandList->ResourceBarrier(1, &barrier);
    }

    void FramePresentationContext::setRenderTargetState(
        ID3D12GraphicsCommandList4* directCommandList)
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        directCommandList->ResourceBarrier(1, &barrier);
    }
}
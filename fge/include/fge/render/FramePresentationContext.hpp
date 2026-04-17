#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>

using namespace Microsoft::WRL;

namespace fge
{
    class FramePresentationContext
    {
    public:
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;
        ComPtr<ID3D12Resource> m_backBuffer;

        FramePresentationContext() = default;
        ~FramePresentationContext();

        FramePresentationContext(const FramePresentationContext&) = default;
        FramePresentationContext& operator=(const FramePresentationContext&) = default;
        FramePresentationContext(FramePresentationContext&&) = default;
        FramePresentationContext& operator=(FramePresentationContext&&) = default;

        void initialize(ComPtr<ID3D12Device5> device, const UINT rtvDescriptorSize,
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle, ComPtr<ID3D12Resource> backBuffer);
        void reset();
        void updateRenderTargetView(ComPtr<ID3D12Device5> device, const UINT rtvDescriptorSize,
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle, ComPtr<ID3D12Resource> backBuffer);
        void resetBackBuffer();
        void setPresentState(ID3D12GraphicsCommandList4* directCommandList);
        void setRenderTargetState(ID3D12GraphicsCommandList4* directCommandList);
    };
}
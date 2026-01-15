#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class FrameContext
    {
    private:
        ComPtr<ID3D12CommandAllocator> m_directCommandAllocator;
    public:
        ComPtr<ID3D12GraphicsCommandList4> m_directCommandList;
        ComPtr<ID3D12Resource> m_backBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;
        uint64_t m_fenceValue;

    public:
        FrameContext() = default;
        ~FrameContext() = default;

        inline uint64_t getFenceValue() const { return m_fenceValue; }

        void initialize(ComPtr<ID3D12Device5> device, UINT rtvDescriptorSize,
            CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, ComPtr<ID3D12Resource> backBuffer);
        void updateRenderTargetView(ComPtr<ID3D12Device5> device, UINT rtvDescriptorSize,
            CD3DX12_CPU_DESCRIPTOR_HANDLE& rtvHandle, ComPtr<ID3D12Resource> backBuffer);
        void resetBackBuffer(uint64_t fenceValue);
        void resetCommand();

        void setPresentState();
        void setRenderTargetState();
    };
}

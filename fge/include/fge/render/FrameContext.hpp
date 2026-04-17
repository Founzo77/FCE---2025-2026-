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
        uint64_t m_fenceValue;

    public:
        FrameContext() = default;
        ~FrameContext() = default;

        inline uint64_t getFenceValue() const { return m_fenceValue; }

        void initialize(ComPtr<ID3D12Device5> device);
        void resetCommand();
    };
}

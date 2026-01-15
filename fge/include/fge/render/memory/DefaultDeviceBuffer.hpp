#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class DefaultDeviceBuffer
    {
    public:
        uint64_t m_size;
        ComPtr<ID3D12Resource> m_buffer;

    public:
        DefaultDeviceBuffer() = default;
        ~DefaultDeviceBuffer() = default;

        DefaultDeviceBuffer(const DefaultDeviceBuffer&) = default;
        DefaultDeviceBuffer& operator=(const DefaultDeviceBuffer&) = default;
        DefaultDeviceBuffer(DefaultDeviceBuffer&&) = default;
        DefaultDeviceBuffer& operator=(DefaultDeviceBuffer&&) = default;

        void initialize(ComPtr<ID3D12Device5> device, uint64_t size, 
            D3D12_RESOURCE_FLAGS resourceFlag, 
            D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON);
        void reallocate(ComPtr<ID3D12Device5> device, uint64_t size, 
            D3D12_RESOURCE_FLAGS resourceFlag, 
            D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON);

        inline uint64_t getSize() const noexcept { return m_size; }
        inline ComPtr<ID3D12Resource> getBuffer() noexcept { return m_buffer; }
    };
}

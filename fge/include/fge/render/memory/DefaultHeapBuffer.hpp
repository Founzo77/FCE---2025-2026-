#pragma once

#include "../../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class DefaultHeapBuffer
    {
    public:
        uint64_t m_totalSize;
        vector<uint64_t> m_offsets;
        ComPtr<ID3D12Heap> m_heap;
        vector<ComPtr<ID3D12Resource>> m_buffers;

    public:
        DefaultHeapBuffer() = default;
        ~DefaultHeapBuffer();

        void initialize(ComPtr<ID3D12Device5> device, uint64_t totalSize,
            D3D12_HEAP_FLAGS heapFlag);
        void allocate(ComPtr<ID3D12Device5> device, uint64_t size,
            D3D12_RESOURCE_FLAGS resourceFlag, D3D12_RESOURCE_STATES initialResourceState);

        void reset();

        inline uint64_t getTotalSize() const noexcept { return m_totalSize; }
        inline uint64_t getNbElements() const noexcept { return m_buffers.size(); }
        inline ComPtr<ID3D12Resource> getBuffer(uint64_t index)
        {
            return m_buffers[index];
        }
    };
}

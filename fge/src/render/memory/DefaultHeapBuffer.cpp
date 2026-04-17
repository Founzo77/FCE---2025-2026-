#include <fge/render/memory/DefaultHeapBuffer.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

namespace fge
{
    DefaultHeapBuffer::~DefaultHeapBuffer()
    {
        reset();
    }

    void DefaultHeapBuffer::initialize(ComPtr<ID3D12Device5> device, uint64_t totalSize,
        D3D12_HEAP_FLAGS heapFlag)
    {
        m_totalSize = totalSize;

        D3D12_HEAP_DESC desc = {};
        desc.SizeInBytes = totalSize;
        desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Flags = heapFlag;

        throwIfFailed(device->CreateHeap(&desc, IID_PPV_ARGS(&m_heap)),
            "Failed to create ID3D12Heap");
    }

    void DefaultHeapBuffer::allocate(ComPtr<ID3D12Device5> device, uint64_t size,
        D3D12_RESOURCE_FLAGS resourceFlag, D3D12_RESOURCE_STATES initialResourceState)
    {
        throwIfFailed((!m_heap) == false, "DefaultHeapBuffer not initialized");

        uint64_t alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        uint64_t offset = 0;

        if(m_offsets.empty() == false)
        {
            offset = m_offsets.back();
            offset += alignData(m_buffers.back()->GetDesc().Width, alignment);
        }

        offset = alignData(offset, alignment);

        throwIfFailed(offset + size <= m_totalSize, "Not enough space in heap to allocate");

        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlag);

        ComPtr<ID3D12Resource> placedResource;
        throwIfFailed(device->CreatePlacedResource(m_heap.Get(), offset,
            &bufferDesc, initialResourceState, nullptr, IID_PPV_ARGS(&placedResource)));

        m_offsets.push_back(offset);
        m_buffers.push_back(placedResource);
    }

    void DefaultHeapBuffer::reset()
    {
        m_totalSize = 0;
        m_offsets.clear();
        m_offsets.shrink_to_fit();
        m_heap.Reset();
        m_buffers.clear();
        m_buffers.shrink_to_fit();
    }
}
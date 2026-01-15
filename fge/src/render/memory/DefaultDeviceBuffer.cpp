#include <fge/render/memory/DefaultDeviceBuffer.hpp>
#include <fge/utility.hpp>

#include <d3dx12.h>


namespace fge
{
    void DefaultDeviceBuffer::initialize(ComPtr<ID3D12Device5> device, uint64_t size, 
        D3D12_RESOURCE_FLAGS resourceFlag, 
        D3D12_RESOURCE_STATES initialResourceState)
    {
        m_size = size;

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size, resourceFlag);
        
        throwIfFailed(device->CreateCommittedResource(&heapProperties,
            D3D12_HEAP_FLAG_NONE, &desc, initialResourceState, nullptr, 
            IID_PPV_ARGS(&m_buffer)));
    }

    void DefaultDeviceBuffer::reallocate(ComPtr<ID3D12Device5> device, uint64_t size, 
        D3D12_RESOURCE_FLAGS resourceFlag, 
        D3D12_RESOURCE_STATES initialResourceState)
    {
        m_buffer.Reset();
        initialize(device, size, resourceFlag, initialResourceState);   
    }
}
#include <fge/resources/BufferFactory.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

namespace fge
{
    ComPtr<ID3D12Resource> BufferFactory::buildUploadBuffer(ComPtr<ID3D12Device5> device, 
        uint64_t size)
    {
        ComPtr<ID3D12Resource> uploadBuffer;

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

        throwIfFailed(device->CreateCommittedResource(&heapProperties, 
            D3D12_HEAP_FLAG_NONE, &bufferDesc, 
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer)),
            "Failed to build Upload Buffer");

        return uploadBuffer;
    }
}

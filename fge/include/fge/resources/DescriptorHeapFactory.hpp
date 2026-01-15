#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class DescriptorHeapFactory
    {
    public:
        static ComPtr<ID3D12DescriptorHeap> buildDescriptiorHeap(
            ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_HEAP_TYPE type, 
            uint32_t nbDescriptors, 
            D3D12_DESCRIPTOR_HEAP_FLAGS descriptorFlag = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    };
}


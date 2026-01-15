#include <fge/resources/DescriptorHeapFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    ComPtr<ID3D12DescriptorHeap> fge::DescriptorHeapFactory::buildDescriptiorHeap(
        ComPtr<ID3D12Device5> device, D3D12_DESCRIPTOR_HEAP_TYPE type, 
            uint32_t nbDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS descriptorFlag)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;
 
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = nbDescriptors;
        desc.Type = type;
        desc.Flags = descriptorFlag;
    
        throwIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)),
            "Failed to create ID3D12DescriptorHeap");
    
        return descriptorHeap;
    }
}

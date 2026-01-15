#include <fge/resources/CommandAllocatorFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    ComPtr<ID3D12CommandAllocator> fge::CommandAllocatorFactory::buildCommandAllocator(
        ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        throwIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)),
            "Failed to create ID3D12CommandAllocator");
    
        return commandAllocator;
    }
}

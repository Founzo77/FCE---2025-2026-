#include <fge/resources/CommandQueueFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    ComPtr<ID3D12CommandQueue> fge::CommandQueueFactory::buildCommandQueue(
        ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandQueue> commandQueue;

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        throwIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)),
            "Failed to create ID3D12CommandQueue");

        return commandQueue;
    }
}

#include <fge/resources/CommandListFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    ComPtr<ID3D12GraphicsCommandList4> fge::CommandListFactory::buildCommandListV4(
        ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;

        throwIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, 
            IID_PPV_ARGS(&commandList)), "Failed to create ID3D12GraphicsCommandList");
        throwIfFailed(commandList->Close(), "Failed to close commandList which was just created");

        ComPtr<ID3D12GraphicsCommandList4> commandList4;
        commandList.As(&commandList4);

        throwIfFailed(commandList4, 
            "Failed to convert ID3D12GraphicsCommandList to ID3D12GraphicsCommandList4");

        return commandList4;
    }
}
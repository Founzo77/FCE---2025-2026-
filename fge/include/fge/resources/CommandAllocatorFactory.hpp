#pragma once

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace fge
{
    class CommandAllocatorFactory
    {
    public:
        static ComPtr<ID3D12CommandAllocator> buildCommandAllocator(
            ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type);
    };
}

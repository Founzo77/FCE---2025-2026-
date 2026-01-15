#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace fge
{
    class CommandQueueFactory
    {
    public:
        static ComPtr<ID3D12CommandQueue> buildCommandQueue(ComPtr<ID3D12Device5> device,
            D3D12_COMMAND_LIST_TYPE type);
    };
}

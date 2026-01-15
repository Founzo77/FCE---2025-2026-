#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace fge
{
    class CommandListFactory
    {
    public:
        static ComPtr<ID3D12GraphicsCommandList4> buildCommandListV4(
            ComPtr<ID3D12Device5> device, ComPtr<ID3D12CommandAllocator> commandAllocator, 
            D3D12_COMMAND_LIST_TYPE type);
    };
}

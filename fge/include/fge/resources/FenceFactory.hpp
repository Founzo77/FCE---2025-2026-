#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace fge
{
    class FenceFactory
    {
    public:
        static ComPtr<ID3D12Fence> buildFence(ComPtr<ID3D12Device5> device);
    };
}

#include <fge/resources/FenceFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    ComPtr<ID3D12Fence> FenceFactory::buildFence(ComPtr<ID3D12Device5> device)
    {
        ComPtr<ID3D12Fence> fence;

        throwIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)),
            "Failed to create ID3D12Fence");

        return fence;
    }
}
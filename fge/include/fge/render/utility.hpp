#pragma once

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    void waitForFenceValue(ComPtr<ID3D12Fence> fence, HANDLE event, uint64_t fenceValue);
    void signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);
}
#include <fge/render/utility.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void waitForFenceValue(ComPtr<ID3D12Fence> fence, HANDLE event, uint64_t fenceValue)
    {
        if (fence->GetCompletedValue() < fenceValue)
        {
            throwIfFailed(fence->SetEventOnCompletion(fenceValue, event));
            WaitForSingleObject(event, INFINITE);
        }
    }

    void signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue)
    {
        fenceValue++;
        throwIfFailed(commandQueue->Signal(fence.Get(), fenceValue));
    }
}
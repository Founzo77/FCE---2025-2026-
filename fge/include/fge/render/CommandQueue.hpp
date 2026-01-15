#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class CommandQueue
    {
    public:
        ComPtr<ID3D12CommandQueue> m_commandQueue;

    public:
        CommandQueue() = default;
        ~CommandQueue() = default;

        void initialize(ComPtr<ID3D12Device5> device, D3D12_COMMAND_LIST_TYPE type);
        void flush(ComPtr<ID3D12Fence> fence, HANDLE event, uint64_t& fenceValue);
    };
}
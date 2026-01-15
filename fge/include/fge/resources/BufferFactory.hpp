#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class BufferFactory
    {
    public:
        static ComPtr<ID3D12Resource> buildUploadBuffer(ComPtr<ID3D12Device5> device,
            uint64_t size);
    };
}

#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;

namespace fge
{
    class UploadBuffer
    {
    public:
        uint64_t m_size;
        ComPtr<ID3D12Resource> m_buffer;

    public:
        UploadBuffer();
        ~UploadBuffer() = default;

        UploadBuffer(const UploadBuffer&) = delete;
        UploadBuffer(UploadBuffer&&) = default;

        UploadBuffer& operator=(const UploadBuffer&) = delete;
        UploadBuffer& operator=(UploadBuffer&&) = default;

        void initialize(ComPtr<ID3D12Device5> device, uint64_t size);
        void reallocate(ComPtr<ID3D12Device5> device, uint64_t size);

        inline uint64_t getSize() const noexcept { return m_size; }
        inline ComPtr<ID3D12Resource> getBuffer() noexcept { return m_buffer; }

        void upload(const void* data, uint64_t size);
        void upload(const void** data, uint64_t* sizes, uint64_t* offsets, uint64_t nbElements);
        void* startUpload();
        void endUpload();
    };
}

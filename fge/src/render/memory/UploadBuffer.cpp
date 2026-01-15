#include <fge/render/memory/UploadBuffer.hpp>

#include <fge/resources/BufferFactory.hpp>
#include <fge/utility.hpp>

#include <d3dx12.h>

namespace fge
{
    UploadBuffer::UploadBuffer() : m_size(0), m_buffer()
    {

    }

    void UploadBuffer::initialize(ComPtr<ID3D12Device5> device, uint64_t size)
    {
        m_size = size;
        m_buffer = BufferFactory::buildUploadBuffer(device, size);
    }

    void UploadBuffer::reallocate(ComPtr<ID3D12Device5> device, uint64_t size)
    {
        m_buffer.Reset();
        initialize(device, size);
    }

    void UploadBuffer::upload(const void* data, uint64_t size)
    {
        void* mappedData = nullptr;

        CD3DX12_RANGE readRange(0, 0);
        throwIfFailed(m_buffer->Map(0, &readRange, &mappedData),
            "Failed to map the upload buffer of ComPtr<ID3D12Resource");
        memcpy(mappedData, data, size);

        m_buffer->Unmap(0, nullptr);
    }

    void UploadBuffer::upload(const void** data, uint64_t* sizes, uint64_t* offsets, 
        uint64_t nbElements)
    {
        uint8_t* mappedData = nullptr;

        CD3DX12_RANGE readRange(0, 0);
        throwIfFailed(m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)),
            "Failed to map the upload buffer of ComPtr<ID3D12Resource");
        
        for(uint64_t i = 0; i < nbElements; i++)
        {
            memcpy(mappedData, data[i], sizes[i]);
            mappedData += offsets[i];
        }

        m_buffer->Unmap(0, nullptr);
    }

    void* UploadBuffer::startUpload()
    {
        void* mappedData = nullptr;

        CD3DX12_RANGE readRange(0, 0);
        throwIfFailed(m_buffer->Map(0, &readRange, &mappedData),
            "Failed to map the upload buffer of ComPtr<ID3D12Resource");

        return mappedData;
    }

    void UploadBuffer::endUpload()
    {
        m_buffer->Unmap(0, nullptr);
    }
}
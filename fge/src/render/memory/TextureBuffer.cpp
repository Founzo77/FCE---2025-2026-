#include <fge/render/memory/TextureBuffer.hpp>

#include <fge/utility.hpp>

#include <DirectXTex.h>
#include <d3dx12.h>

namespace fge
{
    void TextureBuffer::initialize(ComPtr<ID3D12Device5> device, 
        uint32_t width, uint32_t height)
    {
        m_metaData.width = width;
        m_metaData.height = height;
        m_metaData.arraySize = 1;
        m_metaData.mipLevels = 1;
        m_metaData.format = DXGI_FORMAT_R8G8B8A8_UNORM;

        CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1,
            1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
            &textureDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, 
            IID_PPV_ARGS(&m_texture));
    }

    void TextureBuffer::initialize(ComPtr<ID3D12Device5> device, 
        const DirectX::TexMetadata& metaData)
    {
        // TO_DO Specifier initializeReadOnly
        m_metaData = metaData;

        throwIfFailed(m_metaData.format != DXGI_FORMAT_UNKNOWN, 
            "Invalid texture format: DXGI_FORMAT_UNKNOWN");

        CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            m_metaData.format, m_metaData.width, m_metaData.height, 
            m_metaData.arraySize, m_metaData.mipLevels,
            //1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            1, 0, D3D12_RESOURCE_FLAG_NONE);

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
            &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, 
            IID_PPV_ARGS(&m_texture));
    }

    void TextureBuffer::reallocate(ComPtr<ID3D12Device5> device, 
        uint32_t width, uint32_t height)
    {
        m_texture.Reset();
        initialize(device, width, height);
    }

    void TextureBuffer::reallocate(ComPtr<ID3D12Device5> device, 
        const DirectX::TexMetadata& metaData)
    {
        m_texture.Reset();
        initialize(device, metaData);
    }

    void TextureBuffer::createUAV(ComPtr<ID3D12Device5> device, 
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        device->CreateUnorderedAccessView(m_texture.Get(), nullptr, nullptr, cpuHandle);
    }
}
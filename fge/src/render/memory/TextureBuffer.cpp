#include <fge/render/memory/TextureBuffer.hpp>

#include <fge/utility.hpp>

#include <DirectXTex.h>
#include <d3dx12.h>

namespace fge
{
    TextureBuffer::~TextureBuffer()
    {
        reset();
    }

    void TextureBuffer::initialize(ComPtr<ID3D12Device5> device, 
        uint32_t width, uint32_t height, D3D12_RESOURCE_FLAGS flags, DXGI_FORMAT format)
    {
        m_metaData.width = width;
        m_metaData.height = height;
        m_metaData.arraySize = 1;
        m_metaData.mipLevels = 1;
        m_metaData.format = format;

        CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            format, width, height, 1, 1, 1, 0, flags);

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

        CD3DX12_RESOURCE_DESC textureDesc = {};

        switch (m_metaData.dimension)
        {
            case DirectX::TEX_DIMENSION_TEXTURE2D:
            {
                textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
                    m_metaData.format, m_metaData.width, m_metaData.height,
                    static_cast<UINT16>(m_metaData.arraySize), 
                    static_cast<UINT16>(m_metaData.mipLevels),
                    1, 0, D3D12_RESOURCE_FLAG_NONE);
                break;
            }

            case DirectX::TEX_DIMENSION_TEXTURE3D:
            {
                textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(
                    m_metaData.format, m_metaData.width, m_metaData.height,
                    static_cast<UINT16>(m_metaData.depth), static_cast<UINT16>(m_metaData.mipLevels),
                    D3D12_RESOURCE_FLAG_NONE);
                break;
            }
            default:
                throwIfFailed(false, "Unsupported texture dimension");
        }

        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
            &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, 
            IID_PPV_ARGS(&m_texture));
    }

    void TextureBuffer::reallocate(ComPtr<ID3D12Device5> device, 
        uint32_t width, uint32_t height, D3D12_RESOURCE_FLAGS flags,
        DXGI_FORMAT format)
    {
        m_texture.Reset();
        initialize(device, width, height, flags, format);
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
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.Format = m_metaData.format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

        device->CreateUnorderedAccessView(m_texture.Get(), nullptr, &desc, cpuHandle);
    }

    void TextureBuffer::createRTV(ComPtr<ID3D12Device5> device, 
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        device->CreateRenderTargetView(m_texture.Get(), nullptr, cpuHandle);
    }

    void TextureBuffer::reset()
    {
        m_texture.Reset();
        m_metaData = {};
    }
}
#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXTex.h>

#include <cstdint>

using namespace Microsoft::WRL;
using DirectX::TexMetadata;

namespace fge
{
    class TextureBuffer
    {
    public:
        TexMetadata m_metaData;
        ComPtr<ID3D12Resource> m_texture;

    public:
        TextureBuffer() = default;
        ~TextureBuffer();

        TextureBuffer(const TextureBuffer&) = delete;
        TextureBuffer(TextureBuffer&&) = default;

        TextureBuffer& operator=(const TextureBuffer&) = delete;
        TextureBuffer& operator=(TextureBuffer&&) = default;

        void initialize(ComPtr<ID3D12Device5> device, uint32_t width, uint32_t height,
            D3D12_RESOURCE_FLAGS flags=D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        void initialize(ComPtr<ID3D12Device5> device, const DirectX::TexMetadata& metaData);
        void reallocate(ComPtr<ID3D12Device5> device, uint32_t width, uint32_t height,
            D3D12_RESOURCE_FLAGS flags=D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            DXGI_FORMAT format=DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        void reallocate(ComPtr<ID3D12Device5> device, const DirectX::TexMetadata& metaData);

        void createUAV(ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
        void createRTV(ComPtr<ID3D12Device5> device, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);

        void reset();

        inline const TexMetadata& getMetaData() const noexcept { return m_metaData; }
        inline ComPtr<ID3D12Resource> getTexture() noexcept { return m_texture; }
    };
}

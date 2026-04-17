#pragma once

#include "../memory/TextureBuffer.hpp"
#include "../memory/UploadBuffer.hpp"

#include <DirectXTex.h>

#include <string>

using namespace DirectX;

using std::string;

namespace fge
{
    class Texture
    {
    public:
        ScratchImage m_image;
        TexMetadata m_metaData;
        TextureBuffer m_deviceBuffer;
        UploadBuffer m_uploadBuffer;

        Texture() = default;
        ~Texture();
        
        Texture(const Texture&) = delete;
        Texture(Texture&&) = default;

        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = default;

        void initialize(const string& texturePathFile);
        void initializeFromDDS(const string& texturePathFile);
        void initializeFromRawTexture2D(const void* data, 
            uint32_t width, uint32_t height, DXGI_FORMAT format);
        void initializeFromRawVolume(const void* data, 
            uint32_t width, uint32_t height, uint32_t depth, DXGI_FORMAT format);
        void reset();
    };
}
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
        
        Texture(const Texture&) = delete;
        Texture(Texture&&) = default;

        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = default;

        void initialize(const string& texturePathFile);
    };
}
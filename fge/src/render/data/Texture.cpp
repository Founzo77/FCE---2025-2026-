#include <fge/render/data/Texture.hpp>

#include <fge/utility.hpp>

#include <iostream>

namespace fge
{
    Texture::~Texture()
    {
        reset();
    }

    void Texture::initialize(const string& texturePathFile)
    {
        reset();

        std::wstring widePath(texturePathFile.begin(), texturePathFile.end());

        TexMetadata metadata;
        ScratchImage image;

        HRESULT hr = LoadFromWICFile(widePath.c_str(),
            WIC_FLAGS_FORCE_SRGB, &metadata, image);

        if (FAILED(hr))
        {
            std::cerr << "Erreur : impossible de charger la texture : " 
                << texturePathFile << std::endl;
            // TO_DO Laiser seulement throwIfFailed
            throwIfFailed(hr, "Impossible to load the texture");
            return;
        }

        DXGI_FORMAT targetFormat;

        if (IsSRGB(metadata.format))
        {
            targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        }
        else
        {
            targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        if (metadata.format != targetFormat)
        {
            ScratchImage converted;

            hr = DirectX::Convert(image.GetImages(), image.GetImageCount(),
                metadata, targetFormat,
                DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);

            if (FAILED(hr))
            {
                throwIfFailed(hr, "Conversion en RGBA échouée");
            }

            m_image = std::move(converted);
            m_metaData = m_image.GetMetadata();
        }
        else
        {
            m_image = std::move(image);
            m_metaData = metadata;
        }
    }

    void Texture::initializeFromDDS(const std::string& texturePathFile)
    {
        m_image = {};
        m_metaData = {};

        std::wstring widePath(texturePathFile.begin(), texturePathFile.end());

        HRESULT hr = LoadFromDDSFile(widePath.c_str(), DDS_FLAGS_NONE,
            &m_metaData, m_image);

        if (FAILED(hr))
        {
            throwIfFailed(hr, "Impossible de charger la texture 3D DDS");
        }

        if (m_metaData.dimension != TEX_DIMENSION_TEXTURE3D)
        {
            throw std::runtime_error("Le fichier DDS n'est pas une texture 3D");
        }
    }

    void Texture::initializeFromRawTexture2D(const void* data, 
        uint32_t width, uint32_t height, DXGI_FORMAT format)
    {
        m_metaData = {};
        m_metaData.dimension = TEX_DIMENSION_TEXTURE2D;
        m_metaData.width = width;
        m_metaData.height = height;
        m_metaData.depth = 1;
        m_metaData.arraySize = 1;
        m_metaData.mipLevels = 1;
        m_metaData.format = format;

        // Allocation ScratchImage (texture 2D)
        HRESULT hr = m_image.Initialize2D(format, width, height,
            1, // array size
            1  // mip levels
        );
        assert(SUCCEEDED(hr));

        const Image* img = m_image.GetImage(0, 0, 0);

        const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
        uint8_t* dst = img->pixels;

        const size_t srcRowPitch = width * BitsPerPixel(format) / 8;
        const size_t dstRowPitch = img->rowPitch;

        for (uint32_t y = 0; y < height; y++)
        {
            memcpy(dst + y * dstRowPitch, src + y * srcRowPitch, srcRowPitch);
        }
    }

    void Texture::initializeFromRawVolume(
        const void *data, uint32_t width, uint32_t height, uint32_t depth, DXGI_FORMAT format)
    {
        m_metaData = {};
        m_metaData.dimension = TEX_DIMENSION_TEXTURE3D;
        m_metaData.width = width;
        m_metaData.height = height;
        m_metaData.depth = depth;
        m_metaData.arraySize = 1;
        m_metaData.mipLevels = 1;
        m_metaData.format = format;

        // Allocation ScratchImage (volume 3D)
        HRESULT hr = m_image.Initialize3D(format, width, height, depth,
            1 // mip levels
        );
        assert(SUCCEEDED(hr));

        // Copie brute des données
        const Image* img = m_image.GetImage(0, 0, 0);
        size_t slicePitch = img->slicePitch;

        memcpy(img->pixels, data, slicePitch * depth);
    }

    void Texture::reset()
    {
        m_image.Release();
        m_metaData = {};
        m_deviceBuffer.reset();
        m_uploadBuffer.reset();
    }
}

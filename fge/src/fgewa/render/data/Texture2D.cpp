#include <fgewa/render/data/Texture2D.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Texture.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Texture2D::~Texture2D()
    {
        reset();
    }

    Texture2D::Texture2D(Texture2D&& other) : m_textureHandle(other.m_textureHandle),
        m_samplerHandle(other.m_samplerHandle), m_device(other.m_device)
    {
        other.m_textureHandle = nullptr;
        other.m_samplerHandle = nullptr;
    }

    Texture2D& Texture2D::operator=(Texture2D&& other)
    {
        if(this != &other)
        {
            reset();

            m_textureHandle = other.m_textureHandle;
            m_samplerHandle = other.m_samplerHandle;
            m_device = other.m_device;

            other.m_textureHandle = nullptr;
            other.m_samplerHandle = nullptr;            
        }

        return *this;
    }

    void Texture2D::initialize(shared_ptr<Device> device, const fge::Texture& textureData)
    {
        reset();

        throwIfFailed(device != nullptr, "Invalid ANARI device");

        m_device = device;

        const TexMetadata& meta = textureData.m_metaData;
        throwIfFailed(meta.dimension == TEX_DIMENSION_TEXTURE2D, "Only 2D textures supported");

        uint32_t width  = static_cast<uint32_t>(meta.width);
        uint32_t height = static_cast<uint32_t>(meta.height);

        const Image* img = textureData.m_image.GetImage(0, 0, 0);
        throwIfFailed(img != nullptr, "Invalid ScratchImage");

        const void* pixels = img->pixels;

        ANARIDataType anariType = ANARI_UNKNOWN;

        switch(meta.format)
        {
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                anariType = ANARI_UFIXED8_VEC4;
                break;

            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                anariType = ANARI_UFIXED8_VEC4;
                break;

            case DXGI_FORMAT_B8G8R8A8_UNORM:
                anariType = ANARI_UFIXED8_VEC4;
                break;

            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                anariType = ANARI_UFIXED8_VEC4;
                break;

            case DXGI_FORMAT_R16G16B16A16_UNORM:
                anariType = ANARI_UFIXED16_VEC4;
                break;

            default:
                throwIfFailed(false, "Unsupported texture format for ANARI");
        }

        // TO_DO Voir pour le deleter
        m_textureHandle = anariNewArray2D(m_device->getHandle(), pixels,
            nullptr, nullptr, anariType, width, height);

        throwIfFailed(m_textureHandle != nullptr, "Failed to create ANARIArray2D");

        anariCommitParameters(m_device->getHandle(), m_textureHandle);

        m_samplerHandle = anariNewSampler(m_device->getHandle(), "image2D");

        throwIfFailed(m_samplerHandle != nullptr, "Failed to create ANARISampler");

        anariSetParameter(m_device->getHandle(), m_samplerHandle,
            "image", ANARI_ARRAY2D, &m_textureHandle);

        // Wrap modes
        anariSetParameter(m_device->getHandle(), m_samplerHandle,
            "wrapMode1", ANARI_STRING, "repeat");

        anariSetParameter(m_device->getHandle(), m_samplerHandle,
            "wrapMode2", ANARI_STRING, "repeat");

        anariCommitParameters(m_device->getHandle(), m_samplerHandle);
    }

    void Texture2D::reset()
    {
        if (m_samplerHandle)
        {
            anariRelease(m_device->getHandle(), m_samplerHandle);
            m_samplerHandle = nullptr;
        }

        if (m_textureHandle)
        {
            anariRelease(m_device->getHandle(), m_textureHandle);
            m_textureHandle = nullptr;
        }

        m_device = nullptr;
    }

    ANARISampler Texture2D::getSamplerHandle() noexcept
    {
        return m_samplerHandle;
    }

    const ANARISampler Texture2D::getSamplerHandle() const noexcept
    {
        return m_samplerHandle;
    }

    ANARIArray2D Texture2D::getArray2DHandle() noexcept
    {
        return m_textureHandle;
    }

    const ANARIArray2D Texture2D::getArray2DHandle() const noexcept
    {
        return m_textureHandle;
    }
}
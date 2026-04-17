#include <fgewa/render/data/Texture3D.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Texture.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Texture3D::~Texture3D()
    {
        reset();
    }

    Texture3D::Texture3D(Texture3D&& other) : m_texture3DHandle(other.m_texture3DHandle),
        m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth), m_device(other.m_device)
    {
        other.m_texture3DHandle = nullptr;
    }

    Texture3D& Texture3D::operator=(Texture3D&& other)
    {
        if (this != &other)
        {
            reset();

            m_texture3DHandle = other.m_texture3DHandle;
            m_width = other.m_width;
            m_height = other.m_height;
            m_depth = other.m_depth;
            m_device = other.m_device;

            other.m_texture3DHandle = nullptr;
        }

        return *this;
    }

    void Texture3D::initialize(shared_ptr<Device> device, const fge::Texture& textureData)
    {
        reset();

        throwIfFailed(device != nullptr, "Invalid ANARI device");

        m_device = device;

        const TexMetadata& meta = textureData.m_metaData;

        throwIfFailed(meta.dimension == TEX_DIMENSION_TEXTURE3D, "Texture3D requires a 3D texture");

        m_width = static_cast<uint32_t>(meta.width);
        m_height = static_cast<uint32_t>(meta.height);
        m_depth = static_cast<uint32_t>(meta.depth);

        const Image* img = textureData.m_image.GetImage(0, 0, 0);
        throwIfFailed(img != nullptr, "Invalid ScratchImage");

        const void* voxels = img->pixels;

        ANARIDataType anariType = ANARI_UNKNOWN;

        switch(meta.format)
        {
            case DXGI_FORMAT_R8_UNORM:
                anariType = ANARI_UFIXED8;
                break;

            case DXGI_FORMAT_R16_UNORM:
                anariType = ANARI_UFIXED16;
                break;

            case DXGI_FORMAT_R32_FLOAT:
                anariType = ANARI_FLOAT32;
                break;

            default:
                throwIfFailed(false, "Unsupported 3D texture format for ANARI");
        }

        m_texture3DHandle = anariNewArray3D(m_device->getHandle(),
            voxels, nullptr, nullptr, anariType, m_width, m_height, m_depth);

        throwIfFailed(m_texture3DHandle != nullptr, "Failed to create ANARIArray3D");

        anariCommitParameters(m_device->getHandle(), m_texture3DHandle);
    }

    void Texture3D::reset()
    {
        if (m_texture3DHandle)
        {
            anariRelease(m_device->getHandle(), m_texture3DHandle);
            m_texture3DHandle = nullptr;
        }

        m_device = nullptr;
    }

    ANARIArray3D Texture3D::getTexture3DHandle() noexcept
    {
        return m_texture3DHandle;
    }

    const ANARIArray3D Texture3D::getTexture3DHandle() const noexcept
    {
        return m_texture3DHandle;
    }

    uint32_t Texture3D::getWidth() const noexcept
    {
        return m_width;
    }

    uint32_t Texture3D::getHeight() const noexcept
    {
        return m_height;
    }

    uint32_t Texture3D::getDepth() const noexcept
    {
        return m_depth;
    }
}

#include <fgewa/render/data/Volume.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Volume.hpp>
#include <fge/render/data/Texture.hpp>
#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::throwIfFailed;
using fge::globalLogger;

namespace fgewa
{
    Volume::~Volume()
    {
        reset();
    }

    Volume::Volume(Volume&& other) : m_volume(other.m_volume), 
        m_spatialField(other.m_spatialField), m_device(other.m_device)
    {
        other.m_volume = nullptr;
        other.m_spatialField = nullptr;
    }

    Volume& Volume::operator=(Volume&& other)
    {
        if (this != &other)
        {
            reset();

            m_volume = other.m_volume;
            m_spatialField = other.m_spatialField;
            m_device = other.m_device;

            other.m_volume = nullptr;
            other.m_spatialField = nullptr;
        }

        return *this;
    }

    void Volume::initialize(shared_ptr<Device> device, const fge::Volume& volumeData,
        const unordered_map<LogicalIndex, Texture3D>& textures3D,
        const unordered_map<LogicalIndex, fge::Texture>& fgeTextures2D)
    {
        throwIfFailed(device != nullptr, "Invalid ANARI device");
        m_device = device;

        // Retrieve Texture3D
        const Texture3D& texture3d = 
            textures3D.at(LogicalIndex(volumeData.m_volumeData.m_texture3DIndex));

        // Create SpatialField
        m_spatialField = anariNewSpatialField(device->getHandle(), "structuredRegular");

        throwIfFailed(m_spatialField != nullptr, "Failed to create spatial field");

        ANARIArray3D arrayHandle = texture3d.getTexture3DHandle();

        anariSetParameter(device->getHandle(), 
            m_spatialField, "data", ANARI_ARRAY3D, &arrayHandle);

        float origin[3] = { volumeData.m_volumeData.m_mins[0], 
            volumeData.m_volumeData.m_mins[1], volumeData.m_volumeData.m_mins[2]};

        const uint32_t nx = texture3d.getWidth();
        const uint32_t ny = texture3d.getHeight();
        const uint32_t nz = texture3d.getDepth();

        const float minX = volumeData.m_volumeData.m_mins[0];
        const float minY = volumeData.m_volumeData.m_mins[1];
        const float minZ = volumeData.m_volumeData.m_mins[2];

        const float maxX = volumeData.m_volumeData.m_maxs[0];
        const float maxY = volumeData.m_volumeData.m_maxs[1];
        const float maxZ = volumeData.m_volumeData.m_maxs[2];

        float spacing[3] = { (maxX - minX) / float(nx - 1), 
            (maxY - minY) / float(ny - 1),
            (maxZ - minZ) / float(nz - 1) };

        anariSetParameter(device->getHandle(), 
            m_spatialField, "origin", ANARI_FLOAT32_VEC3, origin);
        anariSetParameter(device->getHandle(), 
            m_spatialField, "spacing", ANARI_FLOAT32_VEC3, spacing);

        anariCommitParameters(device->getHandle(), m_spatialField);

        m_volume = anariNewVolume(device->getHandle(), "transferFunction1D");

        throwIfFailed(m_volume != nullptr, "Failed to create volume");

        anariSetParameter(device->getHandle(),
            m_volume, "value", ANARI_SPATIAL_FIELD, &m_spatialField);
        
        uint32_t tfIndex = volumeData.m_volumeData.m_transferFunctionTextureIndex;

        ANARIArray1D colorArray = nullptr;
        ANARIArray1D opacityArray = nullptr;

        std::vector<float> colors;
        std::vector<float> opacities;

        if (tfIndex == UINT32_MAX)
        {
            // Fallback: Default TF
            const int tfSize = 4;

            colors = {
                0.f, 0.f, 0.f,
                0.8f, 0.3f, 0.1f,
                1.f, 0.8f, 0.4f,
                1.f, 1.f, 1.f
            };

            opacities = { 0.0f, 0.05f, 0.2f, 0.6f };

            colorArray = anariNewArray1D(device->getHandle(),
                colors.data(), nullptr, nullptr, ANARI_FLOAT32_VEC3, tfSize);

            opacityArray = anariNewArray1D(device->getHandle(),
                opacities.data(), nullptr, nullptr, ANARI_FLOAT32, tfSize);
        }
        /*
        else
        {
            uint32_t width = 4;
            const uint32_t height = 1;

            throwIfFailed(height >= 1, "Transfer function texture must have at least one row");

            colors.resize(width * 3);
            opacities.resize(width);
            colors = {
                0.f, 0.f, 0.f,
                0.8f, 0.3f, 0.1f,
                1.f, 0.8f, 0.4f,
                1.f, 1.f, 1.f
            };
            opacities = { 0.05f, 0.05f, 0.2f, 0.6f };

            colorArray = anariNewArray1D(device->getHandle(),
                colors.data(), nullptr, nullptr, ANARI_FLOAT32_VEC3, width);
            opacityArray = anariNewArray1D(device->getHandle(),
                opacities.data(), nullptr, nullptr, ANARI_FLOAT32, width);
        }
        */
        else
        {
            const fge::Texture& tfTexture = fgeTextures2D.at(LogicalIndex(tfIndex));

            const Image* img = tfTexture.m_image.GetImage(0, 0, 0);
            throwIfFailed(img != nullptr, "Invalid TF texture");

            uint32_t width = (uint32_t)tfTexture.m_metaData.width;

            const uint8_t* pixels = img->pixels;

            colors.resize(width * 3);
            opacities.resize(width);

            bool isBGRA = tfTexture.m_metaData.format == DXGI_FORMAT_B8G8R8A8_UNORM ||
                tfTexture.m_metaData.format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

            for (uint32_t x = 0; x < width; ++x)
            {
                const uint8_t* px = pixels + x * 4;
                float r, g, b, a;
                if (isBGRA)
                {
                    b = (float)px[0] / 255.f;
                    g = (float)px[1] / 255.f;
                    r = (float)px[2] / 255.f;
                }
                else
                {
                    r = (float)px[0] / 255.f;
                    g = (float)px[1] / 255.f;
                    b = (float)px[2] / 255.f;
                }

                a = (float)px[3] / 255.f;
                colors[x * 3 + 0] = r;
                colors[x * 3 + 1] = g;
                colors[x * 3 + 2] = b;
                opacities[x] = a;
            }

            colorArray = anariNewArray1D(device->getHandle(),
                colors.data(), nullptr, nullptr, ANARI_FLOAT32_VEC3, width);
            opacityArray = anariNewArray1D(device->getHandle(),
                opacities.data(), nullptr, nullptr, ANARI_FLOAT32, width);
        }

        throwIfFailed(colorArray != nullptr, "Failed to create TF color array");
        throwIfFailed(opacityArray != nullptr, "Failed to create TF opacity array");

        anariCommitParameters(device->getHandle(), colorArray);
        anariCommitParameters(device->getHandle(), opacityArray);

        anariSetParameter(device->getHandle(), m_volume, "color", ANARI_ARRAY1D, &colorArray);
        anariSetParameter(device->getHandle(), m_volume, "opacity", ANARI_ARRAY1D, &opacityArray);

        // Scalar range
        float valueRange[2] = {
            volumeData.m_volumeData.m_scalarMin, volumeData.m_volumeData.m_scalarMax};
        
        anariSetParameter(device->getHandle(), 
            m_volume, "valueRange", ANARI_FLOAT32_BOX1, valueRange);
        
        anariCommitParameters(device->getHandle(), m_volume);

        anariRelease(device->getHandle(), colorArray);
        anariRelease(device->getHandle(), opacityArray);
    }

    void Volume::reset()
    {
        if (m_volume)
        {
            anariRelease(m_device->getHandle(), m_volume);
            m_volume = nullptr;
        }

        if (m_spatialField)
        {
            anariRelease(m_device->getHandle(), m_spatialField);
            m_spatialField = nullptr;
        }

        m_device = nullptr;
    }

    ANARIVolume Volume::getVolumeHandle() noexcept
    {
        return m_volume;
    }

    const ANARIVolume Volume::getVolumeHandle() const noexcept
    {
        return m_volume;
    }

}

#include <fgeia/SpatialField.hpp>

#include <fge/render/data/Texture.hpp>

#include <iostream>

namespace fgeia
{
    static DXGI_FORMAT anariToDXGI(anari::DataType type)
    {
        switch (type)
        {
        case ANARI_UFIXED8_VEC4:
        case ANARI_UFIXED8_RGBA_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case ANARI_UFIXED8_VEC3:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        
        case ANARI_UFIXED8:
            return DXGI_FORMAT_R8_UNORM;

        case ANARI_FLOAT32_VEC4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case ANARI_FLOAT32_VEC3:
            return DXGI_FORMAT_R32G32B32_FLOAT;

        case ANARI_FLOAT32:
            return DXGI_FORMAT_R32_FLOAT;

        case ANARI_UFIXED16:
            return DXGI_FORMAT_R16_UNORM;

        case ANARI_FLOAT16:
            return DXGI_FORMAT_R16_FLOAT;

        default:
            throw std::runtime_error("Unsupported texture format");
        }
    }

    SpatialField::SpatialField(FgeGlobalState* globalState) : 
        Object(ANARI_SPATIAL_FIELD, globalState)
    {

    }

    SpatialField* SpatialField::createInstance(std::string_view subtype, 
        FgeGlobalState* globalState)
    {
        if (subtype == "structuredRegular")
            return new StructuredRegularField(globalState);
        else
            return (SpatialField *)new UnknownObject(ANARI_SPATIAL_FIELD, subtype, globalState);
    }

    fge::VolumeData& SpatialField::getFgeVolumeData() noexcept
    {
        return m_fgeVolumeData;
    }

    StructuredRegularField::StructuredRegularField(FgeGlobalState* globalState) :
        SpatialField(globalState), m_data(this)
    {

    }

    fge::LogicalIndex StructuredRegularField::getFgeTexture3DIndex() const noexcept
    {
        return m_fgeTexture3DIndex;
    }

    void StructuredRegularField::commitParameters()
    {
        Object::commitParameters();
        m_data = getParamObject<helium::Array3D>("data");
        m_origin = getParam<helium::float3>("origin", helium::float3(0.f));
        m_spacing = getParam<helium::float3>("spacing", helium::float3(1.f));
        m_dims = m_data->size();
    }

    void StructuredRegularField::finalize()
    {
        if (!m_data)
            return;

        const void* data = m_data->data();

        uint32_t width = (uint32_t) m_data->size(0);
        uint32_t height = (uint32_t) m_data->size(1);
        uint32_t depth = (uint32_t) m_data->size(2);
        DXGI_FORMAT format = anariToDXGI(m_data->elementType());

        if(m_fgeTexture3DIndex.m_index == UINT32_MAX)
        {
            m_fgeTexture3DIndex = globalState()->m_texture3DIndexAllocator.alloc();
        }
        fge::Texture fgeTexture3D;
        fgeTexture3D.initializeFromRawVolume(data, width, height, depth, format);
        globalState()->setTexture3D(std::move(fgeTexture3D), m_fgeTexture3DIndex);

        // Compute scalar range
        float scalarMin = std::numeric_limits<float>::max();
        float scalarMax = std::numeric_limits<float>::lowest();

        const size_t voxelCount = (size_t)width * height * depth;

        if (m_data->elementType() == ANARI_FLOAT32)
        {
            const float* voxels = static_cast<const float*>(data);

            for (size_t i = 0; i < voxelCount; ++i)
            {
                scalarMin = std::min(scalarMin, voxels[i]);
                scalarMax = std::max(scalarMax, voxels[i]);
            }
        }
        else if (m_data->elementType() == ANARI_UFIXED8)
        {
            const uint8_t* voxels = static_cast<const uint8_t*>(data);

            for (size_t i = 0; i < voxelCount; ++i)
            {
                float v = voxels[i] / 255.f;
                scalarMin = std::min(scalarMin, v);
                scalarMax = std::max(scalarMax, v);
            }
        }
        else
        {
            scalarMin = 0.f;
            scalarMax = 1.f;
        }

        // Compute volume bounds
        const anari::math::float3 mins = m_origin;

        const anari::math::float3 maxs = {
            m_origin.x + m_spacing.x * (float)(width  - 1),
            m_origin.y + m_spacing.y * (float)(height - 1),
            m_origin.z + m_spacing.z * (float)(depth  - 1)
        };

        // Fill VolumeData

        m_fgeVolumeData.m_mins = { mins.x, mins.y, mins.z };
        m_fgeVolumeData.m_maxs = { maxs.x, maxs.y, maxs.z };

        m_fgeVolumeData.m_scalarMin = scalarMin;
        m_fgeVolumeData.m_scalarMax = scalarMax;

        m_fgeVolumeData.m_texture3DIndex = m_fgeTexture3DIndex.m_index;

        // Transfer function will be set later by Volume
        m_fgeVolumeData.m_transferFunctionTextureIndex = UINT32_MAX;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::SpatialField*);

#include <fgeia/Volume.hpp>

#include <fge/render/pipeline_state/ShaderBank.hpp>

namespace fgeia
{
    Volume::Volume(FgeGlobalState* globalState) : Object(ANARI_VOLUME, globalState)
    {

    }

    fge::LogicalIndex Volume::getFgeVolumeIndex() const
    {
        return m_fgeVolumeIndex;
    }

    Volume* Volume::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "transferFunction1D")
            return new TransferFunction1D(globalState);
        else
            return (Volume *)new UnknownObject(ANARI_VOLUME, subtype, globalState);
    }

    void Volume::finalize()
    {
        if(m_fgeVolumeIndex == UINT32_MAX)
        {
            m_fgeVolumeIndex = globalState()->m_volumeIndexAllocator.alloc();
        }
    }

    TransferFunction1D::TransferFunction1D(FgeGlobalState* globalState) : Volume(globalState),
        m_field(this), m_colorData(this), m_opacityData(this)
    {

    }

    void TransferFunction1D::commitParameters()
    {
        Volume::commitParameters();
        m_field = getParamObject<SpatialField>("value");
        m_valueRange = getParam<helium::box1>("valueRange", helium::box1{0.f, 1.f});
        m_colorData = getParamObject<helium::Array1D>("color");
        m_uniformColor = anari::math::float4(1.f);
        getParam("color", ANARI_FLOAT32_VEC3, &m_uniformColor);
        getParam("color", ANARI_FLOAT32_VEC4, &m_uniformColor);
        m_opacityData = getParamObject<helium::Array1D>("opacity");
        m_uniformOpacity = getParam<float>("opacity", 1.f) * m_uniformColor.w;
        m_unitDistance = getParam<float>("unitDistance", 1.f);
    }

    void TransferFunction1D::finalize()
    {
        Volume::finalize();

        // TO_DO Set le m_valueRange sur la texture3D
        fge::Volume fgeVolume;
        const SpatialField* spatiaField = dynamic_cast<const SpatialField*>(m_field.get());
        fgeVolume.m_volumeData = m_field.get()->getFgeVolumeData();
        fgeVolume.m_hitGroupIndex = fge::ShaderBank::getHitGroupIndex(L"DvrHitGroup");
        globalState()->setVolume(std::move(fgeVolume), m_fgeVolumeIndex);
    }

    bool TransferFunction1D::isValid() const
    {
        return m_field && m_field->isValid();
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Volume*);

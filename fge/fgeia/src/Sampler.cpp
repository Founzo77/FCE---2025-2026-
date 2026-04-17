#include <fgeia/Sampler.hpp>

namespace fgeia
{
    static DXGI_FORMAT anariToDXGI(anari::DataType type)
    {
        switch (type)
        {
        case ANARI_UFIXED8_VEC4:
        case ANARI_UFIXED8_RGBA_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM;

        case ANARI_FLOAT32_VEC4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case ANARI_FLOAT32_VEC3:
            return DXGI_FORMAT_R32G32B32_FLOAT;

        case ANARI_FLOAT32:
            return DXGI_FORMAT_R32_FLOAT;

        default:
            throw std::runtime_error("Unsupported texture format");
        }
    }

    Sampler::Sampler(FgeGlobalState* globalState) : Object(ANARI_SAMPLER, globalState)
    {

    }

    Sampler* Sampler::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "image1D")
            return new Image1D(globalState);
        else if (subtype == "image2D")
            return new Image2D(globalState);
        else if (subtype == "image3D")
            return new Image3D(globalState);
        else
            return (Sampler*) new UnknownObject(ANARI_SAMPLER, subtype, globalState);
    }

    void Sampler::commitParameters()
    {
        Object::commitParameters();

        m_inAttribute = getParamString("inAttribute", "attribute0");
        m_inOffset = getParam<anari::math::float4>("inOffset", 
            anari::math::float4(0.f, 0.f, 0.f, 0.f));
        m_outOffset = getParam<anari::math::float4>("outOffset", 
            anari::math::float4(0.f, 0.f, 0.f, 0.f));
        m_inTransform = anari::math::identity;
        getParam("inTransform", ANARI_FLOAT32_MAT4, &m_inTransform);
        m_outTransform = anari::math::identity;
        getParam("outTransform", ANARI_FLOAT32_MAT4, &m_outTransform);
    }

    TextureDataSampler::TextureDataSampler(FgeGlobalState* globalState) : Sampler(globalState)
    {
        
    }

    fge::LogicalIndex TextureDataSampler::getFgeIndex() const
    {
        return m_fgeIndex;
    }

    void TextureDataSampler::commitParameters()
    {
        Sampler::commitParameters();
    }

    void TextureDataSampler::finalize()
    {
        if(m_fgeIndex.m_index == UINT32_MAX)
        {
            m_fgeIndex = globalState()->m_textureIndexAllocator.alloc();
        }
    }

    Image1D::Image1D(FgeGlobalState* globalState) : TextureDataSampler(globalState)
    {

    }

    void Image1D::commitParameters()
    {
        TextureDataSampler::commitParameters();
        m_image = getParamObject<helium::Array1D>("image");
        m_wrapMode = getParamString("wrapMode", "clampToEdge");
    }

    void Image1D::finalize()
    {
        if (!m_image)
            return;

        const void* data = m_image->data();
        uint32_t width = (uint32_t)m_image->size();
        DXGI_FORMAT format = anariToDXGI(m_image->elementType());

        TextureDataSampler::finalize();
        fge::Texture fgeTexture;
        fgeTexture.initializeFromRawTexture2D(data, width, 1, format);
        globalState()->setTexture2D(std::move(fgeTexture), m_fgeIndex);
    }

    bool Image1D::isValid() const
    {
        return m_image;
    }

    Image2D::Image2D(FgeGlobalState* globalState) : TextureDataSampler(globalState)
    {

    }

    void Image2D::commitParameters()
    {
        TextureDataSampler::commitParameters();
        m_image = getParamObject<helium::Array2D>("image");
        m_wrapMode1 = getParamString("wrapMode1", "clampToEdge");
        m_wrapMode2 = getParamString("wrapMode2", "clampToEdge");
    }

    void Image2D::finalize()
    {
        if (!m_image)
            return;

        const void* data = m_image->data();

        uint32_t width = (uint32_t)m_image->size(0);
        uint32_t height = (uint32_t)m_image->size(1);
        DXGI_FORMAT format = anariToDXGI(m_image->elementType());

        TextureDataSampler::finalize();
        fge::Texture fgeTexture;
        fgeTexture.initializeFromRawTexture2D(data, width, height, format);
        globalState()->setTexture2D(std::move(fgeTexture), m_fgeIndex);
    }

    bool Image2D::isValid() const
    {
        return m_image;
    }

    Image3D::Image3D(FgeGlobalState* globalState) : TextureDataSampler(globalState)
    {

    }

    void Image3D::commitParameters()
    {
        TextureDataSampler::commitParameters();
        m_image = getParamObject<helium::Array3D>("image");
        m_wrapMode1 = getParamString("wrapMode1", "clampToEdge");
        m_wrapMode2 = getParamString("wrapMode2", "clampToEdge");
        m_wrapMode3 = getParamString("wrapMode3", "clampToEdge");
    }

    void Image3D::finalize()
    {
        if (!m_image)
            return;

        const void* data = m_image->data();

        uint32_t width = (uint32_t)m_image->size(0);
        uint32_t height = (uint32_t)m_image->size(1);
        uint32_t depth = (uint32_t)m_image->size(2);
        DXGI_FORMAT format = anariToDXGI(m_image->elementType());

        if(m_fgeIndex.m_index == UINT32_MAX)
        {
            m_fgeIndex = globalState()->m_texture3DIndexAllocator.alloc();
        }
        fge::Texture fgeTexture;
        fgeTexture.initializeFromRawVolume(data, width, height, depth, format);
        globalState()->setTexture2D(std::move(fgeTexture), m_fgeIndex);
    }

    bool Image3D::isValid() const
    {
        return m_image;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Sampler*);

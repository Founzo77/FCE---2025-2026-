#include <fgeia/Material.hpp>

namespace fgeia
{
    // Helper functions ///////////////////////////////////////////////////////////

    template <typename T>
    inline MaterialParameter<T>
    getMaterialHelper(Object *o, const char *p, T defaultValue)
    {
        MaterialParameter<T> retval;
        retval.value = o->getParam<T>(p, defaultValue);
        retval.attribute = o->getParamString(p, "");
        retval.sampler = o->getParamObject<Sampler>(p);
        return retval;
    }

    template <>
    inline MaterialParameter<anari::math::float4>
    getMaterialHelper(Object *o, const char *p, anari::math::float4 defaultValue)
    {
        MaterialParameter<anari::math::float4> retval;
        retval.value = defaultValue;
        o->getParam(p, ANARI_FLOAT32_VEC3, &retval.value);
        o->getParam(p, ANARI_FLOAT32_VEC4, &retval.value);
        retval.attribute = o->getParamString(p, "");
        retval.sampler = o->getParamObject<Sampler>(p);
        return retval;
    }

    static uint32_t getTextureIndex(const helium::IntrusivePtr<Sampler>& sampler)
    {
        if (!sampler)
            return UINT32_MAX;

        const TextureDataSampler* texSampler = 
            dynamic_cast<const TextureDataSampler*>(sampler.ptr);

        if (!texSampler)
            return UINT32_MAX;

        return texSampler->getFgeIndex().m_index;
    }
    
    Material::Material(FgeGlobalState* globalState) : Object(ANARI_MATERIAL, globalState),
        m_fgeMaterialIndex()
    {

    }

    Material* Material::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "matte")
            return new Matte(globalState);
        else if (subtype == "physicallyBased")
            return new PhysicallyBased(globalState);
        else
            return (Material*) new UnknownObject(ANARI_MATERIAL, subtype, globalState);
    }

    const fge::LogicalIndex& fgeia::Material::getFgeIndex() const noexcept
    {
        return m_fgeMaterialIndex;
    }

    void Material::finalize()
    {
        if(m_fgeMaterialIndex.m_index == UINT32_MAX)
        {
            m_fgeMaterialIndex = globalState()->m_materialIndexAllocator.alloc();
        }
    }

    Matte::Matte(FgeGlobalState* globalState) : Material(globalState)
    {

    }

    void Matte::commitParameters()
    {
        Material::commitParameters();
        m_color = getMaterialHelper(this, "color", anari::math::float4(0.8f, 0.8f, 0.8f, 1.f));
        m_opacity = getMaterialHelper(this, "opacity", 1.f);
    }

    bool Matte::isValid() const
    {
        return !m_color.sampler || m_color.sampler->isValid();
    }

    void Matte::finalize()
    {
        Material::finalize();

        const auto& c = m_color.value;

        XMFLOAT3 reflectance = { 0, 0, 0};
        uint32_t albedoTexture = getTextureIndex(m_color.sampler);
        XMFLOAT3 transmittance = {0.f, 0.f, 0.f};

        float opacity = m_opacity.value;

        if (opacity < 1.f)
        {
            transmittance = { 1.f - opacity, 1.f - opacity, 1.f - opacity};
        }

        fge::Material material(reflectance, transmittance,
            albedoTexture, UINT32_MAX, {0.f, 0.f, 0.f}, 0.f);
        globalState()->setMaterial(material, m_fgeMaterialIndex);
    }

    PhysicallyBased::PhysicallyBased(FgeGlobalState* globalState) : Material(globalState)
    {
        
    }

    void PhysicallyBased::commitParameters()
    {
        Material::commitParameters();

        m_baseColor = getMaterialHelper(this, "baseColor", anari::math::float4(1, 1, 1, 1));
        m_emissive = getMaterialHelper(this, "emissive", anari::math::float3(0, 0, 0));
        m_specularColor = getMaterialHelper(this, "specularColor", anari::math::float3(1, 1, 1));
        m_opacity = getMaterialHelper(this, "opacity", 1.f);
        m_metallic = getMaterialHelper(this, "metallic", 1.f);
        m_roughness = getMaterialHelper(this, "roughness", 1.f);
        m_specular = getMaterialHelper(this, "specular", 0.f);
        m_transmission = getMaterialHelper(this, "transmission", 0.f);
        m_ior = getParam<float>("ior", 1.5f);
    }

    bool PhysicallyBased::isValid() const
    {
        return true;
    }

    void PhysicallyBased::finalize()
    {
        Material::finalize();

        const auto& base = m_baseColor.value;
        const auto& spec = m_specularColor.value;

        uint32_t albedoTexture = getTextureIndex(m_baseColor.sampler);

        float metallic = m_metallic.value;
        float roughness = std::clamp(m_roughness.value, 0.01f, 1.f);

        XMFLOAT3 reflectance;
        float shininess;

        if (metallic >= 0.5f) // Mirror case
        {
            // Dans ton forward : reflectanceStrength > 0.5
            reflectance = {1.f, 1.f, 1.f};

            // Roughness = 0.01 → shininess énorme
            shininess = (2.f / (roughness * roughness)) - 2.f;
        }
        else // Normal object
        {
            float reflectanceStrength = m_specular.value;

            reflectance = {
                spec.x * reflectanceStrength,
                spec.y * reflectanceStrength,
                spec.z * reflectanceStrength
            };

            // Inverse exact de ta formule
            shininess = (2.f / (roughness * roughness)) - 2.f;
        }

        shininess = std::max(1.f, shininess);

        float opacity = m_opacity.value;

        XMFLOAT3 transmittance = {
            (1.f - opacity) * m_transmission.value,
            (1.f - opacity) * m_transmission.value,
            (1.f - opacity) * m_transmission.value};

        fge::Material material(reflectance, transmittance,
            albedoTexture, UINT32_MAX, reflectance, shininess);

        globalState()->setMaterial(material, m_fgeMaterialIndex);
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Material*);

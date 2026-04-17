#include <fgewa/render/data/Material.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Material.hpp>
#include <fge/utility.hpp>

#include <algorithm>

using fge::throwIfFailed;

namespace fgewa
{
    Material::~Material()
    {
        reset();
        m_device = nullptr;
    }

    Material::Material(Material&& other) : m_device(other.m_device), 
        m_material(other.m_material)
    {
        other.m_material = nullptr;
    }

    Material& Material::operator=(Material&& other)
    {
        if(this != &other)
        {
            reset();

            m_device = other.m_device;
            m_material = other.m_material;
            other.m_material = nullptr;
        }

        return *this;
    }

    void Material::initialize(shared_ptr<Device> device, const fge::Material& materialData,
        const unordered_map<LogicalIndex, Texture2D>& textures2D)
    {
        m_device = device;
        throwIfFailed(!m_material, "A Material has already been created");

        m_material = anariNewMaterial(m_device->getHandle(), "physicallyBased");
        throwIfFailed(!m_material == false, "Failed to create ANARI Material");

        float reflectanceStrength = (materialData.m_reflectance.x + materialData.m_reflectance.y 
            + materialData.m_reflectance.z) / 3.f;
        reflectanceStrength = std::clamp(reflectanceStrength, 0.f, 1.f);

        if (materialData.m_albedoTextureIndex != UINT32_MAX)
        {
            ANARISampler sampler = textures2D.at(materialData.m_albedoTextureIndex).getSamplerHandle();
            anariSetParameter(m_device->getHandle(), m_material, "baseColor", ANARI_SAMPLER, &sampler);
        }
        else
        {
            float colors[3] = { 1, 1, 1};
            anariSetParameter(m_device->getHandle(), m_material, "baseColor", ANARI_FLOAT32_VEC3, colors);
        }

        if (materialData.m_normalTextureIndex != UINT32_MAX)
        {
            ANARISampler sampler = textures2D.at(materialData.m_normalTextureIndex).getSamplerHandle();
            anariSetParameter(m_device->getHandle(), m_material, "normal", ANARI_SAMPLER, &sampler);
        }

        if(reflectanceStrength > 0.5) // Mirror
        {
            float metallic = 1.f;
            anariSetParameter(m_device->getHandle(), m_material, "metallic", ANARI_FLOAT32, &metallic);
            float roughness = 0.01f;
            anariSetParameter(m_device->getHandle(), m_material, "roughness", ANARI_FLOAT32, &roughness);
        }
        else // Normal object
        {
            // 0 pour matériaux non métalliques type brique
            float metallic = 0.f;
            anariSetParameter(m_device->getHandle(), m_material, "metallic", ANARI_FLOAT32, &metallic);

            // Roughness (conversion Phong -> PBR)
            float shininess = materialData.m_shininess;
            float roughness = sqrtf(2.f / (shininess + 2.f));
            roughness = std::clamp(roughness, 0.02f, 1.f);

            anariSetParameter(m_device->getHandle(), m_material, "roughness", ANARI_FLOAT32, &roughness);

            // Specular controle par reflectance
            anariSetParameter(m_device->getHandle(), m_material, "specular", ANARI_FLOAT32, &reflectanceStrength);
            anariSetParameter(m_device->getHandle(), m_material, "specularColor", ANARI_FLOAT32_VEC3, 
                &materialData.m_reflectance);
        }

        float transmission = (materialData.m_transmittance.x + materialData.m_transmittance.y +
            materialData.m_transmittance.z) / 3.f;

        if (transmission > 0.01f) // Glass
        {
            anariSetParameter(m_device->getHandle(), m_material, "transmission", ANARI_FLOAT32, &transmission);
            float ior = 1.5f;
            anariSetParameter(m_device->getHandle(), m_material, "ior", ANARI_FLOAT32, &ior);
            float thickness = 1.0f;
            anariSetParameter(m_device->getHandle(), m_material, "thickness", ANARI_FLOAT32, &thickness);
        }

        anariCommitParameters(m_device->getHandle(), m_material);
    }

    void Material::reset()
    {
        if(m_material)
        {
            anariRelease(m_device->getHandle(), m_material);
            m_material = nullptr;
        }
    }

    ANARIMaterial Material::getHandle() noexcept
    {
        return m_material;
    }

    const ANARIMaterial Material::getHandle() const noexcept
    {
        return m_material;
    }

    ANARIMaterial* Material::getMemoryHandle() noexcept
    {
        return &m_material;
    }
}
#include <fgewa/render/data/Material.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Material.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Material::~Material()
    {
        reset();
        m_device = nullptr;
    }

    void Material::initialize(shared_ptr<Device> device, const fge::Material& materialData)
    {
        m_device = device;
        throwIfFailed(!m_material, "A Material has already been created");

        m_material = anariNewMaterial(m_device->getHandle(), "physicallyBased");

        throwIfFailed(!m_material == false, "Failed to create ANARI Material");

        m_material = anariNewMaterial(m_device->getHandle(), "physicallyBased");
        throwIfFailed(m_material == nullptr, "Failed to create ANARI Material");

        float color[3] = {
            materialData.m_reflectance.x,
            materialData.m_reflectance.y,
            materialData.m_reflectance.z
        };

        float transmission[3] = {
            materialData.m_transmittance.x,
            materialData.m_transmittance.y,
            materialData.m_transmittance.z
        };

        float specular[3] = {
            materialData.m_ks.x,
            materialData.m_ks.y,
            materialData.m_ks.z
        };

        // Shininess FGE → roughness = 1 − shininessNorm
        float roughness = 1.f - (materialData.m_shininess / 128.f);

        // 3️⃣ Paramètres ANARI
        anariSetParameter(m_device->getHandle(), m_material, "color",
            ANARI_FLOAT32_VEC3, color);

        anariSetParameter(m_device->getHandle(), m_material, "transmission",
            ANARI_FLOAT32_VEC3, transmission);

        anariSetParameter(m_device->getHandle(), m_material, "specular", 
            ANARI_FLOAT32_VEC3, specular);

        anariSetParameter(m_device->getHandle(), m_material, "roughness",
            ANARI_FLOAT32, &roughness);

        // TO_DO textures : m_albedoTextureIndex, m_normalTextureIndex
        // Ils doivent être reliés à un ANARISampler

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
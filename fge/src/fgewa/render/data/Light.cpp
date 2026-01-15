#include <fgewa/render/data/Light.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Light.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Light::~Light()
    {
        reset();
        m_device = nullptr;
    }

    void Light::initialize(shared_ptr<Device> device, const fge::Light& lightData)
    {
        m_device = device;
        throwIfFailed(!m_light, "A Light has already been created");

        m_light = anariNewLight(m_device->getHandle(), "point");

        throwIfFailed(!m_light == false, "Failed to create ANARI Light");

        float position[3] = {
            lightData.m_position.x,
            lightData.m_position.y,
            lightData.m_position.z
        };
        float radiance[3] = {
            lightData.m_radiance.x,
            lightData.m_radiance.y,
            lightData.m_radiance.z
        };

        anariSetParameter(m_device->getHandle(), m_light, "position", ANARI_FLOAT32_VEC3, position);
        anariSetParameter(m_device->getHandle(), m_light, "intensity", ANARI_FLOAT32_VEC3, radiance);
        //anariSetParameter(m_device->getHandle(), m_light, "radiance", ANARI_FLOAT32_VEC3, radiance);
        anariCommitParameters(m_device->getHandle(), m_light);
    }

    void Light::reset()
    {
        if(m_light)
        {
            anariRelease(m_device->getHandle(), m_light);
            m_light = nullptr;
        }
    }

    ANARILight Light::getHandle() noexcept
    {
        return m_light;
    }

    const ANARILight Light::getHandle() const noexcept
    {
        return m_light;
    }
}
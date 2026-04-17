#include <fgewa/render/data/Light.hpp>
#include <fgewa/render/data/Texture2D.hpp>
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

    Light::Light(Light&& other) : m_light(other.m_light), m_device(other.m_device)
    {
        other.m_light = nullptr;
        other.m_device = nullptr;
    }

    Light& Light::operator=(Light&& other)
    {
        if(this != &other)
        {
            reset();

            m_light = other.m_light;
            m_device = other.m_device;

            other.m_light = nullptr;
            other.m_device = nullptr;
        }

        return *this;
    }

    void Light::initialize(shared_ptr<Device> device, const fge::Light& lightData)
    {
        m_device = device;
        throwIfFailed(!m_light, "A Light has already been created");

        m_light = anariNewLight(m_device->getHandle(), "point");

        throwIfFailed(!m_light == false, "Failed to create ANARI Light");

        float position[3] = {
            lightData.m_position.x, lightData.m_position.y, lightData.m_position.z };
        float color[3] = {
            lightData.m_radiance.x, lightData.m_radiance.y, lightData.m_radiance.z };

        anariSetParameter(m_device->getHandle(), m_light, "position", ANARI_FLOAT32_VEC3, position);
        anariSetParameter(m_device->getHandle(), m_light, "color", ANARI_FLOAT32_VEC3, color);
        float radius = 100;
        anariSetParameter(m_device->getHandle(), m_light, "radius", ANARI_FLOAT32, &radius);

        float intensity = 100.f; // TO_DO commence large pour tester
        anariSetParameter(m_device->getHandle(), m_light, "intensity", ANARI_FLOAT32, &intensity);

        anariCommitParameters(m_device->getHandle(), m_light);
    }

    void Light::initializeHdri(shared_ptr<Device> device, const Texture2D& texture2D)
    {
        m_device = device;
        throwIfFailed(!m_light, "A Light has already been created");

        m_light = anariNewLight(m_device->getHandle(), "hdri");

        throwIfFailed(!m_light == false, "Failed to create ANARI Light");

        ANARIArray2D arrayTexture = texture2D.getArray2DHandle();

        anariSetParameter(m_device->getHandle(), m_light, "radiance", ANARI_ARRAY2D, &arrayTexture);

        float intensity = 1.0f; // pas 100 pour HDRI
        anariSetParameter(m_device->getHandle(), m_light, "intensity", ANARI_FLOAT32, &intensity);

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
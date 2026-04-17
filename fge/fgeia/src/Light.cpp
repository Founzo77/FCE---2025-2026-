#include <fgeia/Light.hpp>

namespace fgeia
{
    Light::Light(FgeGlobalState* globalState) : Object(ANARI_LIGHT, globalState)
    {

    }

    Light* Light::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "point")
            return new Point(globalState);

        return (Light*) new UnknownObject(ANARI_LIGHT, subtype, globalState);
    }

    void Light::commitParameters()
    {
        m_color = getParam<anari::math::float3>("color", anari::math::float3(1.f, 1.f, 1.f));
    }

    void Light::finalize()
    {
        if(m_fgeLightIndex == UINT32_MAX)
        {
            m_fgeLightIndex = globalState()->m_lightIndexAllocator.alloc();
        }
    }

    Point::Point(FgeGlobalState* globalState) : Light(globalState)
    {

    }

    void Point::commitParameters()
    {
        Light::commitParameters();
        m_power = getParam<float>("power", 1.f);
        m_radiance = getParam<float>("radiance", 1.f);
        m_position = getParam<anari::math::float3>("position", anari::math::float3(0.f,0.f,0.f));
        m_intensity = getParam<float>("intensity", NAN);
    }

    void Point::finalize()
    {
        Light::finalize();
        // TO_DO Voir pour intensity et power et m_radiance
        fge::Light light({ m_color.x, m_color.y, m_color.z }, 
            { m_position.x, m_position.y, m_position.z });
        globalState()->setLight(light, m_fgeLightIndex);
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Light*);

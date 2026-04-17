#include <fgeia/Camera.hpp>

namespace fgeia
{
    Camera::Camera(FgeGlobalState* globalState) : Object(ANARI_CAMERA, globalState)
    {

    }

    Camera* Camera::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "perspective")
            return new Perspective(globalState);

        return (Camera*) new UnknownObject(ANARI_CAMERA, subtype, globalState);
    }

    void Camera::commitParameters()
    {
        m_position = getParam<anari::math::float3>("position", anari::math::float3(0.f, 0.f, 0.f));
        if (isnan(m_position.x + m_position.y + m_position.z))
            reportMessage(ANARI_SEVERITY_ERROR, "app set camera.position to NAN coordinates");

        m_direction = anari::math::normalize(
            getParam<anari::math::float3>("direction", anari::math::float3(0.f, 0.f, 1.f)));
        if (isnan(m_direction.x + m_direction.y + m_direction.z))
            reportMessage(ANARI_SEVERITY_ERROR, "app set camera.direction to NAN coordinates");
        
        m_up = anari::math::normalize(
            getParam<anari::math::float3>("up", anari::math::float3(0.f, 1.f, 0.f)));
        if (isnan(m_up.x + m_up.y + m_up.z))
            reportMessage(ANARI_SEVERITY_ERROR, "app set camera.up to NAN coordinates");

        m_imageRegion = anari::math::float4(0.f, 0.f, 1.f, 1.f);
            getParam("imageRegion", ANARI_FLOAT32_BOX2, &m_imageRegion);
    }

    Perspective::Perspective(FgeGlobalState* globalState) : Camera(globalState)
    {

    }

    void Perspective::commitParameters()
    {
        Camera::commitParameters();

        m_aspect = getParam<float>("aspect", 1.f);
        m_fovy = getParam<float>("fovy", anari::radians(60.f));
        m_focusDistance = getParam<float>("focusDistance", 1.f);
        m_apertureRadius = getParam<float>("apertureRadius", 0.f);

        // taille du plan image à distance = focusDistance
        const float physicalHeight = 2.f * tanf(m_fovy * 0.5f);
        const float physicalWidth  = physicalHeight * m_aspect;

        fgeCamera = fge::Camera(
            {m_position.x, m_position.y, m_position.z}, 
            {m_direction.x, m_direction.y, m_direction.z},
            {m_up.x, m_up.y, m_up.z},
            physicalWidth, physicalHeight, m_focusDistance);
    }

    void Perspective::finalize()
    {
        globalState()->setCamera(fgeCamera);
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Camera*);

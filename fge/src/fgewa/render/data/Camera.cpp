#include <fgewa/render/data/Camera.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Camera.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Camera::~Camera()
    {
        reset();
        m_device = nullptr;
    }

    void Camera::initialize(shared_ptr<Device> device, const fge::Camera& cameraData)
    {
        m_device = device;
        throwIfFailed(!m_camera, "A camera has already been created");

        m_camera = anariNewCamera(m_device->getHandle(), "perspective");

        throwIfFailed(!m_camera == false, "Failed to create ANARI camera");

        XMFLOAT3 posF = cameraData.m_position.m_position;
        XMVECTOR forwardV = cameraData.m_position.forward();
        XMVECTOR upV = cameraData.m_position.up();

        XMFLOAT3 forwardF, upF;
        XMStoreFloat3(&forwardF, forwardV);
        XMStoreFloat3(&upF, upV);

        float pos[3] = {posF.x, posF.y, posF.z};
        float dir[3] = {forwardF.x, forwardF.y, forwardF.z};
        float up[3]  = {upF.x, upF.y, upF.z};

        // Position, direction, up → paramètres obligatoires ANARI
        anariSetParameter(m_device->getHandle(), m_camera, "position", ANARI_FLOAT32_VEC3, pos);
        anariSetParameter(m_device->getHandle(), m_camera, "direction", ANARI_FLOAT32_VEC3, dir);
        anariSetParameter(m_device->getHandle(), m_camera, "up", ANARI_FLOAT32_VEC3, up);

        // Aspect ratio = physicalWidth / physicalHeight
        float aspect = cameraData.m_physicalWidth / cameraData.m_physicalHeight;
        anariSetParameter(m_device->getHandle(), m_camera, "aspect", ANARI_FLOAT32, &aspect);

        // FOV → convertis selon ton modèle (exemple : fovy = 45°)
        float fovy = 45.0f * XM_PI / 180.f;
        anariSetParameter(m_device->getHandle(), m_camera, "fovy", ANARI_FLOAT32, &fovy);

        anariCommitParameters(m_device->getHandle(), m_camera);
    }

    void Camera::reset()
    {
        if(m_camera)
        {
            anariRelease(m_device->getHandle(), m_camera);
            m_camera = nullptr;
        }
    }

    ANARICamera Camera::getHandle() noexcept
    {
        return m_camera;
    }

    const ANARICamera Camera::getHandle() const noexcept
    {
        return m_camera;
    }
}
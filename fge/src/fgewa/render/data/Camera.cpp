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

    Camera::Camera(Camera&& other) : m_cameraHandle(other.m_cameraHandle), 
        m_device(std::move(other.m_device)), m_cameraData(other.m_cameraData)
    {

    }

    Camera& Camera::operator=(Camera&& other)
    {
        if(this != &other)
        {
            m_cameraHandle = other.m_cameraHandle;
            m_device = std::move(other.m_device);
            m_cameraData = other.m_cameraData;

            other.m_cameraHandle = nullptr;
        }

        return *this;
    }

    void Camera::initialize(shared_ptr<Device> device, const fge::Camera& cameraData)
    {
        m_device = device;
        m_cameraData = cameraData;
        throwIfFailed(!m_cameraHandle, "A camera has already been created");

        m_cameraHandle = anariNewCamera(m_device->getHandle(), "perspective");
        throwIfFailed(!m_cameraHandle == false, "Failed to create ANARI camera");

        setCameraData(cameraData);
    }

    void Camera::reset()
    {
        if(m_cameraHandle)
        {
            anariRelease(m_device->getHandle(), m_cameraHandle);
            m_cameraHandle = nullptr;
        }
    }

    ANARICamera Camera::getHandle() noexcept
    {
        return m_cameraHandle;
    }

    const ANARICamera Camera::getHandle() const noexcept
    {
        return m_cameraHandle;
    }

    const fge::Camera& Camera::getCameraData() const noexcept
    {
        return m_cameraData;
    }

    fge::Camera& Camera::getCameraData() noexcept
    {
        return m_cameraData;
    }

    void Camera::setCameraData(const fge::Camera& camera)
    {
        m_cameraData = camera;

        XMFLOAT3 posF = m_cameraData.m_position.m_position;
        XMVECTOR forwardV = m_cameraData.m_position.forward();
        XMVECTOR upV = m_cameraData.m_position.up();

        XMFLOAT3 forwardF, upF;
        XMStoreFloat3(&forwardF, forwardV);
        XMStoreFloat3(&upF, upV);

        float pos[3] = {posF.x, posF.y, posF.z};
        float dir[3] = {forwardF.x, forwardF.y, forwardF.z};
        float up[3] = {upF.x, upF.y, upF.z};

        anariSetParameter(m_device->getHandle(), m_cameraHandle, "position", ANARI_FLOAT32_VEC3, pos);
        anariSetParameter(m_device->getHandle(), m_cameraHandle, "direction", ANARI_FLOAT32_VEC3, dir);
        anariSetParameter(m_device->getHandle(), m_cameraHandle, "up", ANARI_FLOAT32_VEC3, up);

        float aspect = m_cameraData.m_physicalWidth / m_cameraData.m_physicalHeight;
        anariSetParameter(m_device->getHandle(), m_cameraHandle, "aspect", ANARI_FLOAT32, &aspect);

        float fovy = 45.0f * XM_PI / 180.f;
        anariSetParameter(m_device->getHandle(), m_cameraHandle, "fovy", ANARI_FLOAT32, &fovy);

        //anariSetParameter(m_device->getHandle(),
            //m_cameraHandle, "stereoMode", ANARI_STRING, "sideBySide");

        anariCommitParameters(m_device->getHandle(), m_cameraHandle);
    }
}
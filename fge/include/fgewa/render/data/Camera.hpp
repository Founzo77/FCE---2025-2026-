#pragma once

#include <fge/render/data/Camera.hpp>

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fgewa
{
    class Device;

    class Camera
    {
    private:
        ANARICamera m_cameraHandle = nullptr;
        fge::Camera m_cameraData;
        shared_ptr<Device> m_device = nullptr;

    public:
        Camera() = default;
        ~Camera();

        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;
        Camera(Camera&& other);
        Camera& operator=(Camera&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Camera& cameraData);
        void reset();
        ANARICamera getHandle() noexcept;
        const ANARICamera getHandle() const noexcept;

        const fge::Camera& getCameraData() const noexcept;
        fge::Camera& getCameraData() noexcept;

        void setCameraData(const fge::Camera& camera);
    };
}
#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fge
{
    class Camera;
}

namespace fgewa
{
    class Device;

    class Camera
    {
    private:
        ANARICamera m_camera = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Camera() = default;
        ~Camera();

        Camera(const Camera&) = default;
        Camera& operator=(const Camera&) = default;
        Camera(Camera&&) = default;
        Camera& operator=(Camera&&) = default;
        
        void initialize(shared_ptr<Device> device, const fge::Camera& cameraData);
        void reset();
        ANARICamera getHandle() noexcept;
        const ANARICamera getHandle() const noexcept;
    };
}
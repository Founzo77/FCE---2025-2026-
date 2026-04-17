#include <fgewa/render/data/Frame.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Camera.hpp>
#include <fgewa/render/data/World.hpp>
#include <fgewa/render/data/Renderer.hpp>
#include <fgewa/render/data/ImplementationCapability.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

#include <iostream>

namespace fgewa
{
    Frame::~Frame()
    {
        reset();
        m_device = nullptr;
    }

    Frame::Frame(Frame&& other) : m_frameHandle(other.m_frameHandle),
        m_camera(std::move(other.m_camera)), m_world(std::move(other.m_world)),
        m_renderer(std::move(other.m_renderer)), m_device(std::move(other.m_device))
    {
        other.m_frameHandle = nullptr;
    }

    Frame& Frame::operator=(Frame&& other)
    {
        if (this != &other)
        {
            reset();

            m_frameHandle = other.m_frameHandle;
            m_camera = std::move(other.m_camera);
            m_world = std::move(other.m_world);
            m_renderer = std::move(other.m_renderer);
            m_device = std::move(other.m_device);

            other.m_frameHandle = nullptr;
        }
        return *this;
    }

    void Frame::initialize(shared_ptr<Device> device, const ImplementationCapability& capability,
        Camera&& camera, World&& world, Renderer&& renderer, 
        const uint32_t width, const uint32_t height)
    {
        m_device = device;
        throwIfFailed(!m_frameHandle, "A Frame has already been created");

        m_frameHandle = anariNewFrame(m_device->getHandle());

        throwIfFailed(m_frameHandle != nullptr, "Failed to create ANARI Frame");

        ANARICamera cameraHandle = camera.getHandle();
        anariSetParameter(m_device->getHandle(),
            m_frameHandle, "camera", ANARI_CAMERA, &cameraHandle);

        ANARIWorld worldHandle = world.getHandle();
        anariSetParameter(m_device->getHandle(),
            m_frameHandle, "world", ANARI_WORLD, &worldHandle);

        ANARIRenderer rendererHandle = renderer.getHandle();
        anariSetParameter(m_device->getHandle(),
            m_frameHandle, "renderer", ANARI_RENDERER, &rendererHandle);

        uint32_t size[2] = { width, height };
        anariSetParameter(m_device->getHandle(), m_frameHandle, "size", ANARI_UINT32_VEC2, size);

        ANARIDataType dataType = ANARI_UFIXED8_VEC4;

        // TO_DO voir le dataType
        if(capability.hasOutputDirectX12Buffer())
        {
            anariSetParameter(m_device->getHandle(), m_frameHandle, 
                "channel.dx12Color", ANARI_DATA_TYPE, &dataType);
        }
        else
        {
            anariSetParameter(m_device->getHandle(), m_frameHandle, 
                "channel.color", ANARI_DATA_TYPE, &dataType);
        }
        
        anariCommitParameters(m_device->getHandle(), m_frameHandle);

        m_camera = std::move(camera);
        m_world = std::move(world);
        m_renderer = std::move(renderer);
    }

    void Frame::reset()
    {
        if (m_frameHandle)
        {
            anariRelease(m_device->getHandle(), m_frameHandle);
            m_frameHandle = nullptr;
        }
    }

    ANARIFrame Frame::getHandle() noexcept
    {
        return m_frameHandle;
    }

    const ANARIFrame Frame::getHandle() const noexcept
    {
        return m_frameHandle;
    }

    const Camera& Frame::getCamera() const noexcept
    {
        return m_camera;
    }

    Camera& Frame::getCamera() noexcept
    {
        return m_camera;
    }

    const World& Frame::getWorld() const noexcept
    {
        return m_world;
    }

    World& Frame::getWorld() noexcept
    {
        return m_world;
    }

    const Renderer& Frame::getRenderer() const noexcept
    {
        return m_renderer;
    }

    Renderer& Frame::getRenderer() noexcept
    {
        return m_renderer;
    }
}
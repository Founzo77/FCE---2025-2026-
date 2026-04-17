#pragma once

#include "Camera.hpp"
#include "World.hpp"
#include "Renderer.hpp"

#include <anari/anari.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace fgewa
{
    class Device;
    class ImplementationCapability;

    class Frame
    {
    private:
        ANARIFrame m_frameHandle = nullptr;
        Camera m_camera;
        World m_world;
        Renderer m_renderer;
        shared_ptr<Device> m_device = nullptr;

    public:
        Frame() = default;
        ~Frame();

        Frame(const Frame&) = delete;
        Frame& operator=(const Frame&) = delete;
        Frame(Frame&& other);
        Frame& operator=(Frame&& other);
        
        void initialize(shared_ptr<Device> device, const ImplementationCapability& capability,
            Camera&& camera, World&& world, Renderer&& renderer, 
            const uint32_t width, const uint32_t height);
        void reset();
        ANARIFrame getHandle() noexcept;
        const ANARIFrame getHandle() const noexcept;

        const Camera& getCamera() const noexcept;
        Camera& getCamera() noexcept;

        const World& getWorld() const noexcept;
        World& getWorld() noexcept;

        const Renderer& getRenderer() const noexcept;
        Renderer& getRenderer() noexcept;
    };
}

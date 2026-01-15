#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fgewa
{
    class Device;

    class Renderer
    {
    private:
        ANARIRenderer m_renderer = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Renderer() = default;
        ~Renderer();

        Renderer(const Renderer&) = default;
        Renderer& operator=(const Renderer&) = default;
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;
        
        void initialize(shared_ptr<Device> device);
        void reset();
        ANARIRenderer getHandle() noexcept;
        const ANARIRenderer getHandle() const noexcept;
    };
}
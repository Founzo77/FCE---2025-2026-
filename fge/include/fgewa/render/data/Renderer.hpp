#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fgewa
{
    class Device;
    class AnariRenderConfig;

    class Renderer
    {
    private:
        ANARIRenderer m_renderer = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Renderer() = default;
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&& other);
        Renderer& operator=(Renderer&& other);
        
        void initialize(shared_ptr<Device> device, const AnariRenderConfig& renderConfig);
        void reset();
        ANARIRenderer getHandle() noexcept;
        const ANARIRenderer getHandle() const noexcept;
        void logAvailableRenderers();
        void logSupportedParameters();
    };
}
#include <fgewa/render/data/Renderer.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/utility.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Renderer::~Renderer()
    {
        reset();
        m_device = nullptr;
    }

    void Renderer::initialize(shared_ptr<Device> device)
    {
        m_device = device;
        throwIfFailed(!m_renderer, "A Renderer has already been created");

        const char **rsubs = anariGetObjectSubtypes(m_device->getHandle(), ANARI_RENDERER);
        const char *rName = hasSubtype(rsubs,"pathtracer") ? "pathtracer" : "default";
        m_renderer = anariNewRenderer(m_device->getHandle(), "perspective");

        throwIfFailed(!m_renderer == false, "Failed to create ANARI Renderer");
    }

    void Renderer::reset()
    {
        if(m_renderer)
        {
            anariRelease(m_device->getHandle(), m_renderer);
            m_renderer = nullptr;
        }
    }

    ANARIRenderer Renderer::getHandle() noexcept
    {
        return m_renderer;
    }

    const ANARIRenderer Renderer::getHandle() const noexcept
    {
        return m_renderer;
    }
}
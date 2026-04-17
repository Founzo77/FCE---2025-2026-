#include <fgewa/render/data/Renderer.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/AnariRenderConfig.hpp>
#include <fgewa/utility.hpp>

#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::throwIfFailed;
using fge::globalLogger;

namespace fgewa
{
    Renderer::~Renderer()
    {
        reset();
        m_device = nullptr;
    }

    Renderer::Renderer(Renderer&& other) : m_renderer(other.m_renderer),
        m_device(std::move(other.m_device))
    {
        other.m_renderer = nullptr;
    }

    Renderer& Renderer::operator=(Renderer&& other)
    {
        if(this != &other)
        {
            reset();

            m_renderer = other.m_renderer;
            m_device = std::move(other.m_device);

            other.m_renderer = nullptr;
        }

        return *this;
    }

    void Renderer::initialize(shared_ptr<Device> device, const AnariRenderConfig& renderConfig)
    {
        m_device = device;
        throwIfFailed(!m_renderer, "A Renderer has already been created");

        const char **rsubs = anariGetObjectSubtypes(m_device->getHandle(), ANARI_RENDERER);
        const char *rName = hasSubtype(rsubs, renderConfig.m_renderAlgorithm.c_str()) 
            ? renderConfig.m_renderAlgorithm.c_str() : "default";
        m_renderer = anariNewRenderer(m_device->getHandle(), rName);

        throwIfFailed(!m_renderer == false, "Failed to create ANARI Renderer");

        float bg[4] = { renderConfig.m_backgroundColor.x, renderConfig.m_backgroundColor.y, 
            renderConfig.m_backgroundColor.z, 1.0f};
        anariSetParameter(m_device->getHandle(), m_renderer, "background", ANARI_FLOAT32_VEC4, bg);
        anariSetParameter(m_device->getHandle(), m_renderer, "ambientRadiance", 
            ANARI_FLOAT32, &renderConfig.m_ambientRadiance);
        
        // TO_DO voir pour les spp
        //int spp = 10;
        //anariSetParameter(m_device->getHandle(), m_renderer, "pixelSamples", ANARI_INT32, &spp);

        anariCommitParameters(m_device->getHandle(), m_renderer);

        globalLogger().info() << "Anari rendering algorithm used: " << rName;

        logAvailableRenderers();
        //logSupportedParameters();
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

    void Renderer::logAvailableRenderers()
    {
        const char **rendererSubtypes = anariGetObjectSubtypes(m_device->getHandle(), ANARI_RENDERER);

        globalLogger().info() << "Available ANARI renderers:";

        for (int i = 0; rendererSubtypes[i] != nullptr; ++i)
        {
            globalLogger().info() << "    - " << rendererSubtypes[i];
        }
    }

    void Renderer::logSupportedParameters()
    {
        throwIfFailed(!m_renderer == false, "Renderer not initialized before querying parameters");

        const ANARIParameter* params = nullptr;

        int result = anariGetProperty(m_device->getHandle(),
            m_renderer, "parameter", ANARI_PARAMETER_LIST, &params, sizeof(ANARIParameter*), ANARI_WAIT);

        if (result == 0 || !params)
        {
            globalLogger().info() << "No parameters reported by this renderer.";
            return;
        }

        globalLogger().info() << "=== Supported Renderer Parameters ===";

        for (int i = 0; params[i].name != nullptr; ++i)
        {
            globalLogger().info() << "Parameter: " << params[i].name;
        }
    }
}
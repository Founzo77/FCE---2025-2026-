#include <fgewa/render/data/Surface.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Geometry.hpp>
#include <fgewa/render/data/Material.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Surface::~Surface()
    {
        reset();
        m_device = nullptr;
    }

    void Surface::initialize(shared_ptr<Device> device, Geometry& geometry, Material& material)
    {
        m_device = device;
        throwIfFailed(!m_surface, "A Surface has already been created");

        m_surface = anariNewSurface(m_device->getHandle());

        throwIfFailed(!m_surface == false, "Failed to create ANARI Surface");

        anariSetParameter(m_device->getHandle(), m_surface, "geometry", ANARI_GEOMETRY,
            geometry.getMemoryHandle());
        anariSetParameter(m_device->getHandle(), m_surface, "material", ANARI_MATERIAL,
            material.getMemoryHandle());
        anariCommitParameters(m_device->getHandle(), m_surface);
    }

    void Surface::reset()
    {
        if(m_surface)
        {
            anariRelease(m_device->getHandle(), m_surface);
            m_surface = nullptr;
        }
    }

    ANARISurface Surface::getHandle() noexcept
    {
        return m_surface;
    }

    const ANARISurface Surface::getHandle() const noexcept
    {
        return m_surface;
    }
}
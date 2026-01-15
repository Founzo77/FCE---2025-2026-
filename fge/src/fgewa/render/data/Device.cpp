#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Library.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Device::~Device()
    {
        reset();
    }

    void Device::initialize(Library& library)
    {
        throwIfFailed(!m_device, "A device has already been loaded");

        m_device = anariNewDevice(library.getHandle());

        throwIfFailed(!m_device == false, "Failed to load device");
    }

    void Device::reset()
    {
        if(m_device)
        {
            m_device = nullptr;
        }
    }

    ANARIDevice Device::getHandle() noexcept
    {
        return m_device;
    }

    const ANARIDevice Device::getHandle() const noexcept
    {
        return m_device;
    }
}
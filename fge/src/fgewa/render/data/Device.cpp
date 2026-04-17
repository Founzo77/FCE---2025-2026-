#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Library.hpp>

#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::globalLogger;
using fge::throwIfFailed;

namespace fgewa
{
    Device::~Device()
    {
        reset();
    }

    Device::Device(Device&& other) : m_device(other.m_device)
    {
        other.m_device = nullptr;
    }

    Device& Device::operator=(Device&& other)
    {
        if(this != &other)
        {
            reset();

            m_device = other.m_device;
            other.m_device = nullptr;
        }

        return *this;
    }

    void Device::initialize(Library& library, ANARIStatusCallback callBack)
    {
        throwIfFailed(!m_device, "A device has already been loaded");

        m_device = anariNewDevice(library.getHandle(), "default");

        anariSetParameter(m_device, m_device, "statusCallback", ANARI_FUNCTION_POINTER, &callBack);
        anariCommitParameters(m_device, m_device);

        throwIfFailed(!m_device == false, "Failed to load device");

        logSupportedExtensions();
    }

    void Device::reset()
    {
        if(m_device)
        {
            anariRelease(m_device, m_device);
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

    void Device::logSupportedExtensions()
    {
        throwIfFailed(!m_device == false, "Device not initialized before querying extensions");

        const char** extensions = nullptr;

        int result = anariGetProperty(m_device, m_device, "extension",
            ANARI_STRING_LIST, &extensions, sizeof(const char**), ANARI_WAIT);

        if (result == 0 || !extensions)
        {
            globalLogger().info()
                << "No extensions supported by this device.";
            return;
        }

        globalLogger().info()
            << "=== Supported ANARI Device Extensions ===";

        for (int i = 0; extensions[i] != nullptr; ++i)
        {
            globalLogger().info()
                << "Extension: " << extensions[i];
        }
    }
}
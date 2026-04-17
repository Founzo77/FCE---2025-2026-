#include <fgewa/render/data/Library.hpp>

#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::throwIfFailed;
using fge::globalLogger;

namespace fgewa
{
    Library::~Library()
    {
        reset();
    }

    Library::Library(Library&& other) : m_library(other.m_library)
    {
        other.m_library = nullptr;
    }

    Library& Library::operator=(Library&& other)
    {
        if(this != &other)
        {
            reset();

            m_library = other.m_library;
            other.m_library = nullptr;
        }

        return *this;
    }

    void Library::initialize(const string& name, ANARIStatusCallback callBack)
    {
        throwIfFailed(!m_library, "A library has already been loaded");

        m_library = anariLoadLibrary(name.c_str(), callBack, nullptr);

        throwIfFailed(!m_library == false, "Failed to load library: " + name);

        logAvailableDevices();
    }

    void Library::reset()
    {
        if(m_library)
        {
            anariUnloadLibrary(m_library);
            m_library = nullptr;
        }
    }

    ANARILibrary Library::getHandle() noexcept
    {
        return m_library;
    }

    const ANARILibrary Library::getHandle() const noexcept
    {
        return m_library;
    }

    void Library::logAvailableDevices()
    {
        throwIfFailed(!m_library == false, "Library not initialized before querying devices");

        const char** devices = anariGetDeviceSubtypes(m_library);

        if (!devices)
        {
            globalLogger().info() << "No devices available in this library.";
            return;
        }

        globalLogger().info() << "=== Available ANARI Devices ===";

        for (int i = 0; devices[i] != nullptr; ++i)
        {
            globalLogger().info() << "Device subtype: " << devices[i];
        }
    }
}
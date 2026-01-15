#include <fgewa/render/data/Library.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Library::~Library()
    {
        reset();
    }

    void Library::initialize(const string& name, ANARIStatusCallback callBack)
    {
        throwIfFailed(!m_library, "A library has already been loaded");

        m_library = anariLoadLibrary(name.c_str(), callBack, nullptr);

        throwIfFailed(!m_library == false, "Failed to load library: " + name);
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
}
#include <fgeia/Library.hpp>
#include <fgeia/Device.hpp>

#include "anari_library_fgeia_queries.h"

namespace fgeia
{
    Library::Library(void* library, ANARIStatusCallback defaultStatusCB, const void* statusCBPtr) :
        anari::LibraryImpl(library, defaultStatusCB, statusCBPtr)
    {
        
    }

    ANARIDevice Library::newDevice(const char* subtype)
    {
        return (ANARIDevice) new Device(this_library(), subtype);
    }

    const char** Library::getDeviceExtensions(const char* deviceType)
    {
        return query_extensions();
    }
}
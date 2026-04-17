#pragma once

#include <anari/backend/LibraryImpl.h>

namespace fgeia
{
    class Library : public anari::LibraryImpl
    {
    private:
        

    public:
        Library(void* library, ANARIStatusCallback defaultStatusCB, const void* statusCBPtr);
        virtual ~Library() override = default;

        ANARIDevice newDevice(const char* subtype) override;
        const char** getDeviceExtensions(const char* deviceType) override;
    };
}

extern "C" ANARI_DEFINE_LIBRARY_ENTRYPOINT(fgeia, handle, scb, scbPtr)
{
    return (ANARILibrary) new fgeia::Library(handle, scb, scbPtr);
}

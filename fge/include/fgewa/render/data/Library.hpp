#pragma once

#include <anari/anari.h>

#include <string>

using std::string;

namespace fgewa
{
    class Library
    {
    private:
        ANARILibrary m_library = nullptr;

    public:
        Library() = default;
        ~Library();

        Library(const Library&) = delete;
        Library& operator=(const Library&) = delete;
        Library(Library&& other);
        Library& operator=(Library&& other);
        
        void initialize(const string& name, ANARIStatusCallback callBack);
        void reset();
        ANARILibrary getHandle() noexcept;
        const ANARILibrary getHandle() const noexcept;
        void logAvailableDevices();
    };
}
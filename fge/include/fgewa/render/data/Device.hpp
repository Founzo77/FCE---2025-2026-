#pragma once

#include <anari/anari.h>

namespace fgewa
{
    class Library;

    class Device
    {
    private:
        ANARIDevice m_device = nullptr;

    public:
        Device() = default;
        ~Device();

        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;
        Device(Device&& other);
        Device& operator=(Device&& other);
        
        void initialize(Library& library, ANARIStatusCallback callBack);
        void reset();
        ANARIDevice getHandle() noexcept;
        const ANARIDevice getHandle() const noexcept;
        void logSupportedExtensions();
    };
}
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

        Device(const Device&) = default;
        Device& operator=(const Device&) = default;
        Device(Device&&) = default;
        Device& operator=(Device&&) = default;
        
        void initialize(Library& library);
        void reset();
        ANARIDevice getHandle() noexcept;
        const ANARIDevice getHandle() const noexcept;
    };
}
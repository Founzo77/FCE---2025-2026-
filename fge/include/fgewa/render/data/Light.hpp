#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fge
{
    class Light;
}

namespace fgewa
{
    class Device;

    class Light
    {
    private:
        ANARILight m_light = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Light() = default;
        ~Light();

        Light(const Light&) = default;
        Light& operator=(const Light&) = default;
        Light(Light&&) = default;
        Light& operator=(Light&&) = default;
        
        void initialize(shared_ptr<Device> device, const fge::Light& lightData);
        void reset();
        ANARILight getHandle() noexcept;
        const ANARILight getHandle() const noexcept;
    };
}
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
    class Texture2D;

    class Light
    {
    private:
        ANARILight m_light = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Light() = default;
        ~Light();

        Light(const Light&) = delete;
        Light& operator=(const Light&) = delete;
        Light(Light&& other);
        Light& operator=(Light&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Light& lightData);
        void initializeHdri(shared_ptr<Device> device, const Texture2D& texture2D);
        void reset();
        ANARILight getHandle() noexcept;
        const ANARILight getHandle() const noexcept;
    };
}
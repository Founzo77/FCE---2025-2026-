#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fgewa
{
    class Device;
    class Geometry;
    class Material;

    class Surface
    {
    private:
        ANARISurface m_surface = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Surface() = default;
        ~Surface();

        Surface(const Surface&) = default;
        Surface& operator=(const Surface&) = default;
        Surface(Surface&&) = default;
        Surface& operator=(Surface&&) = default;
        
        void initialize(shared_ptr<Device> device, Geometry& geometry, Material& material);
        void reset();
        ANARISurface getHandle() noexcept;
        const ANARISurface getHandle() const noexcept;
    };
}
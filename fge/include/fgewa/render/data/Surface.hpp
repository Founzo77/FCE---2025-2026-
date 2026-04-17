#pragma once

#include "Geometry.hpp"
#include "Material.hpp"

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fgewa
{
    class Device;

    class Surface
    {
    private:
        ANARISurface m_surface = nullptr;
        Geometry m_geometry;
        Material m_material;
        shared_ptr<Device> m_device = nullptr;

    public:
        Surface() = default;
        ~Surface();

        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;
        Surface(Surface&& other);
        Surface& operator=(Surface&& other);
        
        void initialize(shared_ptr<Device> device, Geometry&& geometry, Material&& material);
        void reset();
        ANARISurface getHandle() noexcept;
        const ANARISurface getHandle() const noexcept;
    };
}
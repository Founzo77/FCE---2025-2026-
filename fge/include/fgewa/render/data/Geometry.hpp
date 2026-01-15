#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fge
{
    class Mesh;
}

namespace fgewa
{
    class Device;

    class Geometry
    {
    private:
        ANARIGeometry m_geometry = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Geometry() = default;
        ~Geometry();

        Geometry(const Geometry&) = default;
        Geometry& operator=(const Geometry&) = default;
        Geometry(Geometry&&) = default;
        Geometry& operator=(Geometry&&) = default;
        
        void initialize(shared_ptr<Device> device, const fge::Mesh& meshData);
        void reset();
        ANARIGeometry getHandle() noexcept;
        const ANARIGeometry getHandle() const noexcept;
        ANARIGeometry* getMemoryHandle() noexcept;
    };
}
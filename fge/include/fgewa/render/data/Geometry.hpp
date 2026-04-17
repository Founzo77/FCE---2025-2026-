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

        Geometry(const Geometry&) = delete;
        Geometry& operator=(const Geometry&) = delete;
        Geometry(Geometry&& other);
        Geometry& operator=(Geometry&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Mesh& meshData, const uint32_t subMeshId);
        void reset();
        ANARIGeometry getHandle() noexcept;
        const ANARIGeometry getHandle() const noexcept;
        ANARIGeometry* getMemoryHandle() noexcept;
    };
}
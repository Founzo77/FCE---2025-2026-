#pragma once

#include "Surface.hpp"
#include "Volume.hpp"

#include <anari/anari.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace fgewa
{
    class Device;
    class Surface;

    class Group
    {
    private:
        ANARIGroup m_group = nullptr;
        ANARIArray1D m_handleSurfaces = nullptr;
        vector<Surface> m_surfaces;
        ANARIArray1D m_handleVolumes = nullptr;
        vector<Volume> m_volumes;
        shared_ptr<Device> m_device = nullptr;
        std::vector<ANARIVolume> m_volumeHandles;
        std::vector<ANARISurface> m_surfaceHandles;

    public:
        Group() = default;
        ~Group();

        Group(const Group&) = delete;
        Group& operator=(const Group&) = delete;
        Group(Group&& other);
        Group& operator=(Group&& other);
        
        void initialize(shared_ptr<Device> device, vector<Surface>&& surfaces);
        void initialize(shared_ptr<Device> device, vector<Volume>&& volumes);
        void reset();
        ANARIGroup getHandle() noexcept;
        const ANARIGroup getHandle() const noexcept;
    };
}
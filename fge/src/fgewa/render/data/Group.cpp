#include <fgewa/render/data/Group.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Surface.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Group::~Group()
    {
        reset();
        m_device = nullptr;
    }

    Group::Group(Group&& other) : m_group(other.m_group), m_handleSurfaces(other.m_handleSurfaces), 
        m_surfaces(std::move(other.m_surfaces)), m_handleVolumes(other.m_handleVolumes),
        m_volumes(std::move(other.m_volumes)), m_device(other.m_device), 
        m_volumeHandles(std::move(other.m_volumeHandles)), m_surfaceHandles(std::move(other.m_surfaceHandles))
    {
        other.m_group = nullptr;
        other.m_handleSurfaces = nullptr;
        other.m_handleVolumes = nullptr;
    }

    Group& Group::operator=(Group&& other)
    {
        if(this != &other)
        {
            reset();

            m_group = other.m_group;
            m_handleSurfaces = other.m_handleSurfaces;
            m_surfaces = std::move(other.m_surfaces);
            m_handleVolumes = other.m_handleVolumes;
            m_volumes = std::move(other.m_volumes);
            m_volumeHandles = std::move(other.m_volumeHandles);
            m_surfaceHandles = std::move(other.m_surfaceHandles);
            m_device = other.m_device;

            other.m_group = nullptr;
            other.m_handleSurfaces = nullptr;
            other.m_handleVolumes = nullptr;
        }

        return *this;
    }

    void Group::initialize(shared_ptr<Device> device, vector<Surface>&& surfaces)
    {
        m_device = device;
        throwIfFailed(!m_group, "A Group has already been created");

        m_group = anariNewGroup(m_device->getHandle());

        throwIfFailed(!m_group == false, "Failed to create ANARI Group");

        m_handleSurfaces = nullptr;
        if (!surfaces.empty())
        {
            m_surfaceHandles.clear();
            m_surfaceHandles.reserve(surfaces.size());
            
            for (Surface& surface : surfaces)
                m_surfaceHandles.push_back(surface.getHandle());

            m_handleSurfaces = anariNewArray1D(m_device->getHandle(),
                m_surfaceHandles.data(), 0, nullptr, ANARI_SURFACE, m_surfaceHandles.size());

            anariSetParameter(m_device->getHandle(), 
                m_group, "surface", ANARI_ARRAY1D, &m_handleSurfaces);
        }

        anariCommitParameters(m_device->getHandle(), m_group);

        m_surfaces = std::move(surfaces);
    }

    void Group::initialize(shared_ptr<Device> device, vector<Volume>&& volumes)
    {
        m_device = device;
        throwIfFailed(!m_group, "A Group has already been created");

        m_group = anariNewGroup(m_device->getHandle());

        throwIfFailed(!m_group == false, "Failed to create ANARI Group");

        m_handleVolumes = nullptr;

        if (!volumes.empty())
        {
            m_volumeHandles.clear();
            m_volumeHandles.reserve(volumes.size());

            for (Volume& volume : volumes)
                m_volumeHandles.push_back(volume.getVolumeHandle());

            m_handleVolumes = anariNewArray1D(m_device->getHandle(),
                m_volumeHandles.data(), 0, nullptr, ANARI_VOLUME, m_volumeHandles.size());

            throwIfFailed(!m_handleVolumes == false,
                "Failed to create ANARI Volume array");
            
            anariSetParameter(m_device->getHandle(), m_group, "volume", ANARI_ARRAY1D, &m_handleVolumes);
        }

        anariCommitParameters(m_device->getHandle(), m_group);

        m_volumes = std::move(volumes);
    }

    void Group::reset()
    {
        if (!m_device)
            return;
            
        if (m_handleSurfaces)
        {
            anariRelease(m_device->getHandle(), m_handleSurfaces);
            m_handleSurfaces = nullptr;
        }

        if (m_handleVolumes)
        {
            anariRelease(m_device->getHandle(), m_handleVolumes);
            m_handleVolumes = nullptr;
        }

        if(m_group)
        {
            anariRelease(m_device->getHandle(), m_group);
            m_group = nullptr;
        }
    }

    ANARIGroup Group::getHandle() noexcept
    {
        return m_group;
    }

    const ANARIGroup Group::getHandle() const noexcept
    {
        return m_group;
    }
}
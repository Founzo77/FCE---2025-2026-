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

    void Group::initialize(shared_ptr<Device> device, vector<Surface>& surfaces)
    {
        m_device = device;
        throwIfFailed(!m_group, "A Group has already been created");

        m_group = anariNewGroup(m_device->getHandle());

        throwIfFailed(!m_group == false, "Failed to create ANARI Group");

        ANARIArray1D surfArray = nullptr;
        if (!surfaces.empty())
        {
            std::vector<ANARISurface> surfHandles;
            surfHandles.reserve(surfaces.size());
            for (auto* s : surfaces)
                surfHandles.push_back(s->getHandle());

            surfArray = anariNewArray1D(m_device->getHandle(),
                                        surfHandles.data(), 0, nullptr,
                                        ANARI_SURFACE,
                                        surfHandles.size());

            anariSetParameter(m_device->getHandle(), m_group,
                            "surface", ANARI_ARRAY1D, &surfArray);
        }

        anariCommitParameters(m_device->getHandle(), m_group);

        // 4️⃣ L’API conserve une ref → on peut release nos Arrays
        if (surfArray)
            anariRelease(m_device->getHandle(), surfArray);
    }

    void Group::reset()
    {
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
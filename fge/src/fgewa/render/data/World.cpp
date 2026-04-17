#include <fgewa/render/data/World.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Instance.hpp>
#include <fgewa/render/data/Light.hpp>

#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    World::~World()
    {
        reset();
        m_device = nullptr;
    }

    World::World(World&& other) : m_worldHandle(other.m_worldHandle),
        m_handleInstances(other.m_handleInstances), m_handleLights(other.m_handleLights),
        m_device(std::move(other.m_device))
    {
        other.m_worldHandle = nullptr;
        other.m_handleInstances = nullptr;
        other.m_handleLights = nullptr;
    }

    World& World::operator=(World&& other)
    {
        if (this != &other)
        {
            reset();

            m_worldHandle = other.m_worldHandle;
            m_handleInstances = other.m_handleInstances;
            m_handleLights = other.m_handleLights;
            m_device = std::move(other.m_device);

            other.m_worldHandle = nullptr;
            other.m_handleInstances = nullptr;
            other.m_handleLights = nullptr;
        }
        
        return *this;
    }

    void World::initialize(shared_ptr<Device> device, 
        vector<Instance>& instances, vector<Light>& lights)
    {
        m_device = device;
        throwIfFailed(!m_worldHandle, "A World has already been created");

        // Création du World ANARI
        m_worldHandle = anariNewWorld(m_device->getHandle());
        throwIfFailed(m_worldHandle != nullptr, "Failed to create ANARI World");

        // Array1D des Instances
        m_handleInstances = nullptr;
        if (!instances.empty())
        {
            std::vector<ANARIInstance> instanceHandles;
            instanceHandles.reserve(instances.size());

            for (Instance& instance : instances)
                instanceHandles.push_back(instance.getHandle());

            m_handleInstances = anariNewArray1D(m_device->getHandle(),
                instanceHandles.data(), 0, nullptr, ANARI_INSTANCE, instanceHandles.size());

            throwIfFailed(m_handleInstances != nullptr, 
                "Failed to create ANARI Instance Array");

            anariSetParameter(m_device->getHandle(),
                m_worldHandle, "instance", ANARI_ARRAY1D, &m_handleInstances);
        }

        // Array1D des Lights
        m_handleLights = nullptr;
        if (!lights.empty())
        {
            std::vector<ANARILight> lightHandles;
            lightHandles.reserve(lights.size());

            for (Light& light : lights)
                lightHandles.push_back(light.getHandle());

            m_handleLights = anariNewArray1D(m_device->getHandle(),
                lightHandles.data(), 0, nullptr, ANARI_LIGHT, lightHandles.size());

            throwIfFailed(m_handleLights != nullptr, "Failed to create ANARI Light Array");

            anariSetParameter(m_device->getHandle(),
                m_worldHandle, "light", ANARI_ARRAY1D, &m_handleLights);
        }

        // Commit final
        anariCommitParameters(m_device->getHandle(), m_worldHandle);
    }

    void World::reset()
    {
        if (m_handleInstances)
        {
            anariRelease(m_device->getHandle(), m_handleInstances);
            m_handleInstances = nullptr;
        }

        if (m_handleLights)
        {
            anariRelease(m_device->getHandle(), m_handleLights);
            m_handleLights = nullptr;
        }

        if (m_worldHandle)
        {
            anariRelease(m_device->getHandle(), m_worldHandle);
            m_worldHandle = nullptr;
        }
    }

    ANARIWorld World::getHandle() noexcept
    {
        return m_worldHandle;
    }

    const ANARIWorld World::getHandle() const noexcept
    {
        return m_worldHandle;
    }
}
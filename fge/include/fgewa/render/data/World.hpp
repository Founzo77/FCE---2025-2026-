#pragma once

#include "Group.hpp"

#include <anari/anari.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace fgewa
{
    class Device;
    class Instance;
    class Light;

    class World
    {
    private:
        ANARIWorld m_worldHandle = nullptr;
        ANARIArray1D m_handleInstances;
        ANARIArray1D m_handleLights;
        shared_ptr<Device> m_device = nullptr;

    public:
        World() = default;
        ~World();

        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&& other);
        World& operator=(World&& other);
        
        void initialize(shared_ptr<Device> device, vector<Instance>& instances,
            vector<Light>& lights);
        void reset();
        ANARIWorld getHandle() noexcept;
        const ANARIWorld getHandle() const noexcept;
    };
}
#pragma once

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
        shared_ptr<Device> m_device = nullptr;

    public:
        Group() = default;
        ~Group();

        Group(const Group&) = default;
        Group& operator=(const Group&) = default;
        Group(Group&&) = default;
        Group& operator=(Group&&) = default;
        
        void initialize(shared_ptr<Device> device, vector<Surface>& surfaces);
        void reset();
        ANARIGroup getHandle() noexcept;
        const ANARIGroup getHandle() const noexcept;
    };
}
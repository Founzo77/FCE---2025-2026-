#pragma once

#include "Group.hpp"

#include <DirectXMath.h>

#include <anari/anari.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

using namespace DirectX;

namespace fge
{
    class Instance;
}

namespace fgewa
{
    class Device;

    class Instance
    {
    private:
        ANARIInstance m_instanceHandle = nullptr;
        Group m_group;
        shared_ptr<Device> m_device = nullptr;

    public:
        Instance() = default;
        ~Instance();

        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;
        Instance(Instance&& other);
        Instance& operator=(Instance&& other);
        
        void initialize(shared_ptr<Device> device, Group&& group, 
            const fge::Instance& instanceData);
        void setTransformMatrix(const XMMATRIX& transform);
        void reset();
        ANARIInstance getHandle() noexcept;
        const ANARIInstance getHandle() const noexcept;
    };
}
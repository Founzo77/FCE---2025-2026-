#include <fgewa/render/data/Instance.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/Group.hpp>

#include <fge/render/data/Instance.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Instance::~Instance()
    {
        reset();
        m_device = nullptr;
    }

    Instance::Instance(Instance&& other)
        : m_instanceHandle(other.m_instanceHandle),
          m_group(std::move(other.m_group)),
          m_device(std::move(other.m_device))
    {
        other.m_instanceHandle = nullptr;
    }

    Instance& Instance::operator=(Instance&& other)
    {
        if (this != &other)
        {
            reset();

            m_instanceHandle = other.m_instanceHandle;
            m_group = std::move(other.m_group);
            m_device = std::move(other.m_device);

            other.m_instanceHandle = nullptr;
        }
        return *this;
    }

    void Instance::initialize(shared_ptr<Device> device, Group&& group, 
        const fge::Instance& instanceData)
    {
        m_device = device;
        throwIfFailed(!m_instanceHandle, "An Instance has already been created");

        m_instanceHandle = anariNewInstance(m_device->getHandle(), "instance");

        throwIfFailed(m_instanceHandle != nullptr, "Failed to create ANARI Instance");

        ANARIGroup groupHandle = group.getHandle();

        XMFLOAT4X4 transformMatrix;
        XMStoreFloat4x4(&transformMatrix, instanceData.m_transform);

        anariSetParameter(m_device->getHandle(),
            m_instanceHandle, "transform", ANARI_FLOAT32_MAT4, &transformMatrix);

        anariSetParameter(m_device->getHandle(),
            m_instanceHandle, "group", ANARI_GROUP, &groupHandle);

        anariCommitParameters(m_device->getHandle(), m_instanceHandle);

        m_group = std::move(group);
    }

    void Instance::setTransformMatrix(const XMMATRIX& transform)
    {
        XMFLOAT4X4 transformMatrix;
        XMStoreFloat4x4(&transformMatrix, transform);
        anariSetParameter(m_device->getHandle(),
            m_instanceHandle, "transform", ANARI_FLOAT32_MAT4, &transformMatrix);
        anariCommitParameters(m_device->getHandle(), m_instanceHandle);
    }

    void Instance::reset()
    {
        if (m_instanceHandle)
        {
            anariRelease(m_device->getHandle(), m_instanceHandle);
            m_instanceHandle = nullptr;
        }
    }

    ANARIInstance Instance::getHandle() noexcept
    {
        return m_instanceHandle;
    }

    const ANARIInstance Instance::getHandle() const noexcept
    {
        return m_instanceHandle;
    }
}
#pragma once

#include "DeviceCamera.hpp"
#include "IntegratorType.hpp"

namespace fge
{
    struct alignas(16) DeviceConstantInformations
    {
    public:
        XMFLOAT4 m_position;
        XMFLOAT4 m_forward;
        XMFLOAT4 m_up;
        XMFLOAT4 m_right;
        float m_physicalWidth;
        float m_physicalHeight;
        float m_depth;
        uint32_t m_nbLights;
        uint32_t m_backgroundTextureIndex;
        IntegratorType m_integratorType;
        uint32_t m_spp;
        uint32_t m_sppIndex;

    public:
        DeviceConstantInformations() = default;
        ~DeviceConstantInformations() = default;

        DeviceConstantInformations(const DeviceConstantInformations&) = default;
        DeviceConstantInformations& operator=(const DeviceConstantInformations&) = default;
        DeviceConstantInformations(DeviceConstantInformations&&) = default;
        DeviceConstantInformations& operator=(DeviceConstantInformations&&) = default;
    };
}
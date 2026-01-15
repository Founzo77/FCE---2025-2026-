#pragma once

#include "Camera.hpp"

#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>

using namespace Microsoft::WRL;
using namespace DirectX;

namespace fge
{
    struct alignas(16) DeviceCamera
    {
    public:
        XMFLOAT4 m_position;
        XMFLOAT4 m_forward;
        XMFLOAT4 m_up;
        XMFLOAT4 m_right;
        float m_physicalWidth;
        float m_physicalHeight;
        float m_depth;
        float _padding;

    public:
        DeviceCamera() = default;
        ~DeviceCamera() = default;

        DeviceCamera(const Camera& camera)
        {
            XMStoreFloat4(&m_position, XMVectorSetW(
                XMLoadFloat3(&camera.m_position.m_position), 1.0f));
            XMStoreFloat4(&m_forward, XMVectorSetW(
                XMLoadFloat3(&camera.m_position.m_basis.m_forward), 1.0f));
            XMStoreFloat4(&m_up, XMVectorSetW(XMLoadFloat3(
                &camera.m_position.m_basis.m_up), 1.0f));
            XMStoreFloat4(&m_right, XMVectorSetW(XMLoadFloat3(
                &camera.m_position.m_basis.m_right), 1.0f));

            m_physicalWidth  = camera.m_physicalWidth;
            m_physicalHeight = camera.m_physicalHeight;
            m_depth = camera.m_depth;
            _padding = 0.0f;
        }

        DeviceCamera(const DeviceCamera&) = default;
        DeviceCamera& operator=(const DeviceCamera&) = default;
        DeviceCamera(DeviceCamera&&) = default;
        DeviceCamera& operator=(DeviceCamera&&) = default;
    };
}
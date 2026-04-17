#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace fce
{
    struct RemoteRequestTeleportation
    {
    public:
        XMFLOAT3 m_position;

        RemoteRequestTeleportation() = default;
        RemoteRequestTeleportation(XMFLOAT3 position) : m_position(position) {}

        RemoteRequestTeleportation(const RemoteRequestTeleportation&) = default;
        RemoteRequestTeleportation(RemoteRequestTeleportation&&) = default;

        RemoteRequestTeleportation& operator=(const RemoteRequestTeleportation&) = default;
        RemoteRequestTeleportation& operator=(RemoteRequestTeleportation&&) = default;
    };
}
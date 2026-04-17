#pragma once

#include <variant>

#include "RemoteRequestTeleportation.hpp"

namespace fce
{
    enum class RemoteRequestType
    {
        TELEPORTATION = 0
    };

    struct RemoteRequest
    {
    public:
        RemoteRequestType m_type;
        std::variant<RemoteRequestTeleportation> m_data;

        RemoteRequest() = default;

        RemoteRequest(const RemoteRequest&) = default;
        RemoteRequest(RemoteRequest&&) = default;

        RemoteRequest& operator=(const RemoteRequest&) = default;
        RemoteRequest& operator=(RemoteRequest&&) = default;
    };
}
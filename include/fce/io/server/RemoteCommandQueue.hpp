#pragma once

#include "RemoteRequest.hpp"

#include <queue>
#include <mutex>
#include <optional>

using std::queue;

namespace fce
{
    class RemoteCommandQueue
    {
    private:
        queue<RemoteRequest> m_queue;
        std::mutex m_mutex;

    public:
        RemoteCommandQueue() = default;

        void push(RemoteRequest&& request);
        std::optional<RemoteRequest> pop();
    };
}

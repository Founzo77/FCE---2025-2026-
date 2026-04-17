#include <fce/io/server/RemoteCommandQueue.hpp>

namespace fce
{
    void RemoteCommandQueue::push(RemoteRequest&& request)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(request));
    }

    std::optional<RemoteRequest> RemoteCommandQueue::pop()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return std::nullopt;

        RemoteRequest request = std::move(m_queue.front());
        m_queue.pop();
        
        return request;
    }
}
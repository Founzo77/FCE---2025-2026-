#pragma once

#include "RemoteController.hpp"

#include <httplib.h>

#include <memory>
#include <thread>
#include <atomic>

using std::unique_ptr;
using std::shared_ptr;

namespace fce
{
    class RemoteCommandQueue;

    class HttplibRemoteController : public RemoteController
    {
    private:
        unique_ptr<httplib::Server> m_server;
        std::thread m_thread;
        std::atomic<bool> m_running = false;
        shared_ptr<RemoteCommandQueue> m_queue;

    public:
        HttplibRemoteController(const string& address, const string& port, 
            shared_ptr<RemoteCommandQueue> queue);

        void run() override;
        void stop() override;
    };
}
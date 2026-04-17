#include <fce/io/server/HttplibRemoteController.hpp>
#include <fce/io/server/RemoteRequestTeleportation.hpp>
#include <fce/io/server/RemoteRequest.hpp>
#include <fce/io/server/RemoteCommandQueue.hpp>

#include <fge/io/GlobalLogger.hpp>
#include <fge/utility.hpp>

#include <nlohmann/json.hpp>

using fge::globalLogger;
using fge::throwIfFailed;

namespace fce
{
    HttplibRemoteController::HttplibRemoteController(const string& address, const string& port,
        shared_ptr<RemoteCommandQueue> queue) : RemoteController(address, port), m_queue(queue)
    {
        globalLogger().debug()
            << "[RemoteController] Created ("
            << m_listenAddress << ":" << m_listenPort << ")";
    }

    void HttplibRemoteController::run()
    {
        m_server = std::make_unique<httplib::Server>();

        m_server->Post("/teleport", [&](const httplib::Request& req, httplib::Response& res)
        {
            globalLogger().debug() << "[RemoteController] POST /teleport from " << req.remote_addr;

            try
            {
                auto json = nlohmann::json::parse(req.body);

                RemoteRequestTeleportation tpData;
                tpData.m_position.x = json["args"]["x"];
                tpData.m_position.y = json["args"]["y"];
                tpData.m_position.z = json["args"]["z"];

                RemoteRequest request;
                request.m_type = RemoteRequestType::TELEPORTATION;
                request.m_data = tpData;

                m_queue->push(std::move(request));
                res.status = 200;
                res.set_content("OK", "text/plain");
            }
            catch (const std::exception& e)
            {
                globalLogger().error()
                    << "[RemoteController] Invalid teleport request: "
                    << e.what()
                    << " Body: " << req.body;

                res.status = 400;
                res.set_content("Invalid JSON", "text/plain");
            }
        });

        m_running = true;

        m_thread = std::thread([&]()
        {
            globalLogger().info() << "[RemoteController] HTTP thread started";

            if (!m_server->listen(m_listenAddress.c_str(), std::stoi(m_listenPort)))
            {
                globalLogger().critical()
                    << "[RemoteController] Failed to bind to "
                    << m_listenAddress << ":" << m_listenPort;

                throwIfFailed(false);
            }

            globalLogger().info() << "[RemoteController] HTTP thread stopped";
        });
    }

    void HttplibRemoteController::stop()
    {
        if (!m_running) 
            return;

        globalLogger().info() << "[RemoteController] Stopping HTTP server";

        m_server->stop();
        if (m_thread.joinable())
            m_thread.join();

        m_running = false;

        globalLogger().info() << "[RemoteController] HTTP server stopped";
    }
}
#pragma once

#include <string>

using std::string;

namespace fce
{
    class RemoteController
    {
    protected:
        string m_listenAddress;
        string m_listenPort;

    public:
        RemoteController(const string& listenAddress, const string& listenPort)
            : m_listenAddress(listenAddress), m_listenPort(listenPort) {}

        virtual ~RemoteController() = default;

        virtual void run() = 0;
        virtual void stop() = 0;

        const string& getListenAddress() const { return m_listenAddress; }
        const string& getListenPort() const { return m_listenPort; }
    };
}
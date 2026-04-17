#pragma once

#include "UpdateContext.hpp"
#include "objects/ObjectManager.hpp"
#include "io/server/RemoteController.hpp"

#include <fge/FgeApplication.hpp>

#include <fge/PlatformWindows.hpp>

#include <string>
#include <memory>

using std::string;
using std::unique_ptr;

namespace fce
{
    class Application
    {
    protected:
        unique_ptr<fge::FgeApplication> m_renderApplication;
        ObjectManager m_objectManager;
        unique_ptr<RemoteController> m_remoteController;
        shared_ptr<RemoteCommandQueue> m_remoteCommandQueue;
        UpdateContext m_updateContext;

    public:
        Application() = default;
        ~Application() = default;

        void initialize(HWND hWnd, uint32_t width, uint32_t height, const string& pathFileScene);

        const UpdateContext& getUpdateContext();

        void update();
        void onKeyDown(int vk);
        void onKeyUp(int vk);
        void onMouseButtonDown(int button);
        void onMouseButtonUp(int button);
        void onRawMouseDelta(LONG dx, LONG dy);
        void onMouseWheel(short delta);
        void onLoseFocus();
        void onHasFocus();
        
        void resize(const uint32_t width, const uint32_t height);
        void stopEngine();
    };
}
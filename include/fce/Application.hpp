#pragma once

#include "UpdateContext.hpp"
#include "objects/ObjectManager.hpp"

#include <fge/Application.hpp>

#include <Windows.h>

#include <string>

using std::string;

namespace fce
{
    class Application
    {
    private:
        fge::Application m_renderApplication;
        ObjectManager m_objectManager;
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
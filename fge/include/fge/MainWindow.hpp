#pragma once

#include "FgeApplication.hpp"

#include <Windows.h>

#include <memory>
#include <string>

using std::shared_ptr;
using std::unique_ptr;
using std::string;

namespace fge
{
    class Scene;

    class MainWindow
    {
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;
        uint32_t m_width;
        uint32_t m_height;
        string m_pathFileScene;
        bool m_isInit;
        bool m_keys[256] = {};
        unique_ptr<FgeApplication> m_application;
        shared_ptr<FgeScene> m_scene;

    public:
        MainWindow(HINSTANCE hInstance, uint32_t width, uint32_t height,
            const string& pathFileScene) noexcept;
        ~MainWindow() = default;

        bool initialize();
        int run();

    private:
        LRESULT messageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        bool registerWindowClass();
        bool createAppWindow();

        void update();

        void setBorderlessScreen();

        static LRESULT CALLBACK windowHandler(HWND hWnd, UINT message, 
            WPARAM wParam, LPARAM lParam);
    };
}

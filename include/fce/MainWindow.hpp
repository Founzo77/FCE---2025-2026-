#pragma once

#include "Application.hpp"

#include <Windows.h>

#include <string>

using std::string;

namespace fce
{
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
        Application m_application;
        bool m_isMouseLocked = true;

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
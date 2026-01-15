#include <fce/MainWindow.hpp>

#include <iostream>
#include <chrono>

namespace fce
{
    MainWindow::MainWindow(HINSTANCE hInstance, uint32_t width, uint32_t height,
        const string& pathFileScene) noexcept :
        m_hInstance(hInstance), m_width(width), m_height(height), 
        m_pathFileScene(pathFileScene), m_isInit(false)
    {

    }

    // TO_DO Fonction de class
    void centerMouse(HWND hwnd)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);

        POINT pt;
        pt.x = (rect.right - rect.left) / 2;
        pt.y = (rect.bottom - rect.top) / 2;

        ClientToScreen(hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
    }

    bool MainWindow::initialize()
    {
        if(registerWindowClass() == false)
            return false;

        createAppWindow();
        //setBorderlessScreen();
        m_application.initialize(m_hWnd, m_width, m_height, m_pathFileScene);
        ShowCursor(FALSE);
        centerMouse(m_hWnd);

        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01; 
        rid.usUsage = 0x02; 
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = m_hWnd;

        RegisterRawInputDevices(&rid, 1, sizeof(rid));

        m_isInit = true;
        return true;
    }

    int MainWindow::run()
    {
        MSG message = {};

        while(message.message != WM_QUIT)
        {
            while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            if (m_isInit)
            {
                update();
                m_application.update();

                if(m_isMouseLocked && GetForegroundWindow() == m_hWnd)
                    centerMouse(m_hWnd);
            }
        }

        m_application.stopEngine();

        return static_cast<int>(message.wParam);
    }

    LRESULT MainWindow::messageHandler(HWND hWnd, UINT message, 
        WPARAM wParam, LPARAM lParam)
    {
        if(m_isInit)
        {
            switch (message)
            {
            case WM_KEYDOWN:
                m_application.onKeyDown((int)wParam);
                return 0;

            case WM_KEYUP:
                m_application.onKeyUp((int)wParam);
                return 0;

            case WM_LBUTTONDOWN:
                m_application.onMouseButtonDown(0);
                return 0;

            case WM_LBUTTONUP:
                m_application.onMouseButtonUp(0);
                return 0;

            case WM_RBUTTONDOWN:
                m_application.onMouseButtonDown(1);
                return 0;

            case WM_RBUTTONUP:
                m_application.onMouseButtonUp(1);
                return 0;

            case WM_MBUTTONDOWN:
                m_application.onMouseButtonDown(2);
                return 0;

            case WM_MBUTTONUP:
                m_application.onMouseButtonUp(2);
                return 0;

            case WM_MOUSEWHEEL:
                m_application.onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
                return 0;

            case WM_INPUT:
            {
                UINT size = sizeof(RAWINPUT);
                static BYTE buffer[sizeof(RAWINPUT)];

                if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size,
                    sizeof(RAWINPUTHEADER)) != size)
                    break;

                RAWINPUT* raw = (RAWINPUT*)buffer;

                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    LONG dx = raw->data.mouse.lLastX;
                    LONG dy = raw->data.mouse.lLastY;

                    m_application.onRawMouseDelta(dx, dy);
                }

                return 0;
            }


            case WM_PAINT:
                ValidateRect(hWnd, nullptr);
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            case WM_SIZE:
            {
                // TO_DO gerer le redimenssionnement
                RECT clientRect = {};
                ::GetClientRect(m_hWnd, &clientRect);
    
                uint32_t width = clientRect.right - clientRect.left;
                uint32_t height = clientRect.bottom - clientRect.top;

                if(width != m_width || height != m_height)
                {
                    m_width = width;
                    m_height = height;
                    m_application.resize(width, height);
                }

                return 0;
            }

            case WM_ACTIVATE:
            {
                if(LOWORD(wParam) == WA_INACTIVE)
                {
                    m_isMouseLocked = false;
                    ShowCursor(TRUE);
                    ClipCursor(NULL);
                    m_application.onLoseFocus();
                }
                else
                {
                    m_isMouseLocked = true;
                    ShowCursor(FALSE);
                    //ClipCursor(windowRect);
                    m_application.onHasFocus();
                }
                return 0;

            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    bool MainWindow::registerWindowClass()
    {
        WNDCLASSEX wClass = {};
        wClass.cbSize = sizeof(wClass);
        wClass.style = CS_HREDRAW | CS_VREDRAW;
        wClass.lpfnWndProc = windowHandler;
        wClass.hInstance = m_hInstance;
        wClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wClass.lpszClassName = "FGE";

        return RegisterClassEx(&wClass);
    }

    bool MainWindow::createAppWindow()
    {
        RECT rect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        m_hWnd = CreateWindowEx(0, "FGE", "FGE", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, m_hInstance, this);

        if(!m_hWnd)
            return false;

        ShowWindow(m_hWnd, SW_SHOW);
        UpdateWindow(m_hWnd);

        return true;
    }

    void MainWindow::update()
    {
        static uint64_t frameCounter = 0;
        static double elapsedSeconds = 0.0;
        static std::chrono::high_resolution_clock clock;
        static auto t0 = clock.now();

        auto t1 = clock.now();
        double deltaTime = std::chrono::duration<double>(t1 - t0).count();
        t0 = t1;

        frameCounter++;
        elapsedSeconds += deltaTime;
        if (elapsedSeconds > 1.0)
        {
            auto fps = frameCounter / elapsedSeconds;
            std::cout << "FPS: " << fps << std::endl;

            frameCounter = 0;
            elapsedSeconds = 0.0;
        }
    }

    void MainWindow::setBorderlessScreen()
    {
        RECT desktopRect;
        GetClientRect(GetDesktopWindow(), &desktopRect);

        LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
        style &= ~(WS_OVERLAPPEDWINDOW); // supprime les styles classiques
        style |= WS_POPUP;               // fenetre popup borderless
        SetWindowLong(m_hWnd, GWL_STYLE, style);

        int screenWidth  = desktopRect.right - desktopRect.left;
        int screenHeight = desktopRect.bottom - desktopRect.top;

        // mettre à jour les membres width/height
        m_width = static_cast<uint32_t>(screenWidth);
        m_height = static_cast<uint32_t>(screenHeight);

        SetWindowPos(
            m_hWnd,
            HWND_TOP,
            desktopRect.left,
            desktopRect.top,
            screenWidth,
            screenHeight,
            SWP_FRAMECHANGED | SWP_NOOWNERZORDER
        );
    }

    LRESULT CALLBACK MainWindow::windowHandler(HWND hWnd, UINT message, WPARAM wParam, 
        LPARAM lParam)
    {
        MainWindow* mainWindow = nullptr;

        if(message == WM_NCCREATE)
        {
            CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
            mainWindow = reinterpret_cast<MainWindow*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(mainWindow));
            mainWindow->m_hWnd = hWnd;
        }
        else
        {
            mainWindow = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if(mainWindow)
        {
            return mainWindow->messageHandler(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
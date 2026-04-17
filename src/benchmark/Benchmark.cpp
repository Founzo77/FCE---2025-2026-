#include <fce/benchmark/Benchmark.hpp>

#include <iostream>
#include <chrono>

namespace fce
{
    Benchmark::Benchmark(HINSTANCE hInstance, uint32_t width, uint32_t height,
        const string& pathFileScene, BenchmarkConfig benchmarkConfig) noexcept :
        m_hInstance(hInstance), m_width(width), m_height(height), 
        m_pathFileScene(pathFileScene), m_benchmarkConfig(benchmarkConfig), m_isInit(false)
    {

    }

    // TO_DO Fonction de class
    static void centerMouse(HWND hwnd)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);

        POINT pt;
        pt.x = (rect.right - rect.left) / 2;
        pt.y = (rect.bottom - rect.top) / 2;

        ClientToScreen(hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
    }

    bool Benchmark::initialize()
    {
        if(registerWindowClass() == false)
            return false;

        createAppWindow();
        //setBorderlessScreen();
        m_application.initialize(m_hWnd, m_width, m_height, m_pathFileScene, m_benchmarkConfig);
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

    int Benchmark::run()
    {
        MSG message = {};

        m_startTime = std::chrono::high_resolution_clock::now();
        m_isBenchmarkRunning = true;

        while(message.message != WM_QUIT && m_isBenchmarkRunning)
        {
            while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }

            if (m_isInit)
            {
                update();

                if(m_isMouseLocked && GetForegroundWindow() == m_hWnd)
                    centerMouse(m_hWnd);
            }
        }

        m_application.stopBenchmark();
        DestroyWindow(m_hWnd);

        return static_cast<int>(message.wParam);
    }

    LRESULT Benchmark::messageHandler(HWND hWnd, UINT message, 
        WPARAM wParam, LPARAM lParam)
    {
        if(m_isInit)
        {
            switch (message)
            {
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
                    //m_application.resize(width, height);
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
                }
                else
                {
                    m_isMouseLocked = true;
                    ShowCursor(FALSE);
                    //ClipCursor(windowRect);
                }
                return 0;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    bool Benchmark::registerWindowClass()
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

    bool Benchmark::createAppWindow()
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

    void Benchmark::update()
    {
        if (!m_isBenchmarkRunning)
            return;

        m_application.update();

        auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - m_startTime).count();

        if (elapsed >= m_benchmarkConfig.m_duration)
        {
            std::cout << "Benchmark finished after " << elapsed << " seconds.\n";
            m_isBenchmarkRunning = false;
        }
    }

    void Benchmark::setBorderlessScreen()
    {
        RECT desktopRect;
        GetClientRect(GetDesktopWindow(), &desktopRect);

        LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
        style &= ~(WS_OVERLAPPEDWINDOW); // supprime les styles classiques
        style |= WS_POPUP; // fenetre popup borderless
        SetWindowLong(m_hWnd, GWL_STYLE, style);

        int screenWidth  = desktopRect.right - desktopRect.left;
        int screenHeight = desktopRect.bottom - desktopRect.top;

        // mettre à jour les membres width/height
        m_width = static_cast<uint32_t>(screenWidth);
        m_height = static_cast<uint32_t>(screenHeight);

        SetWindowPos(m_hWnd, HWND_TOP, desktopRect.left, desktopRect.top,
            screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    }

    LRESULT CALLBACK Benchmark::windowHandler(HWND hWnd, UINT message, WPARAM wParam, 
        LPARAM lParam)
    {
        Benchmark* benchmark = nullptr;

        if(message == WM_NCCREATE)
        {
            CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
            benchmark = reinterpret_cast<Benchmark*>(createStruct->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(benchmark));
            benchmark->m_hWnd = hWnd;
        }
        else
        {
            benchmark = reinterpret_cast<Benchmark*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if(benchmark)
        {
            return benchmark->messageHandler(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
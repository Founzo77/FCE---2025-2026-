#include <fge/MainWindow.hpp>

#include <fge/Application.hpp>
#include <fge/render/Scene.hpp>

#include <fge/io/XmlReader.hpp>

#include <chrono>
#include <iostream>
#include <random>

namespace fge
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> distRadiance(0.0f, 1.0f);
    static std::uniform_real_distribution<float> distPos(0.0f, 25.0f);
    static std::uniform_real_distribution<float> distPos01(0.0f, 10.0f);

    MainWindow::MainWindow(HINSTANCE hInstance, uint32_t width, uint32_t height,
        const string& pathFileScene) noexcept :
        m_hInstance(hInstance), m_width(width), m_height(height), 
        m_pathFileScene(pathFileScene), m_isInit(false)
    {

    }

    bool MainWindow::initialize()
    {
        if(registerWindowClass() == false)
            return false;
        else
        {
            createAppWindow();
            //setBorderlessScreen();
            m_application = std::make_unique<Application>();
            XmlReader xmlReader(m_pathFileScene);
            m_application->initializeSystem();
            m_application->initializeMainRenderer(
                m_hWnd, m_width, m_height, xmlReader.m_sceneDescription);
            m_scene = m_application->getScene();
            m_isInit = true;

            return true;
        }
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
                m_application->update();
            }
        }

        m_application->stopEngine();

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
                m_keys[wParam & 0xFF] = true;
                return 0;

            case WM_KEYUP:
                m_keys[wParam & 0xFF] = false;
                return 0;

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
                    m_application->resize(width, height);
                }

                return 0;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        else
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

    /*
    * z, q, s, d : Deplacement
    * a, e : rotation camera
    * r : ajout de lumières
    * f : déplacement d'une instance
    * v : ajout d'instances
    */
    void MainWindow::update()
    {
        static uint64_t frameCounter = 0;
        static double elapsedSeconds = 0.0;
        static std::chrono::high_resolution_clock clock;
        static auto t0 = clock.now();

        auto t1 = clock.now();
        double deltaTime = std::chrono::duration<double>(t1 - t0).count();
        t0 = t1;

        float moveSpeed = 2.5f;        // vitesse en unités/sec
        float rotateSpeed = XM_PI / 1.f;

        if (m_keys[VK_SHIFT])
        {
            moveSpeed *= 2;
        }

        if (m_keys['Z']) m_scene->moveCameraForward(moveSpeed * deltaTime);
        if (m_keys['S']) m_scene->moveCameraForward(-moveSpeed * deltaTime);
        if (m_keys['Q']) m_scene->moveCameraRight(-moveSpeed * deltaTime);
        if (m_keys['D']) m_scene->moveCameraRight(moveSpeed * deltaTime);

        if (m_keys['A']) m_scene->rotateCameraY(rotateSpeed * deltaTime);
        if (m_keys['E']) m_scene->rotateCameraY(-rotateSpeed * deltaTime);

        if (m_keys[VK_SPACE]) m_scene->moveCameraUp(moveSpeed * deltaTime);
        if (m_keys[VK_CONTROL]) m_scene->moveCameraUp(-moveSpeed * deltaTime);

        if (m_keys['R'])
        {
            static uint32_t indexLight = 20;
            indexLight++;

            XMFLOAT3 radiance(
                distRadiance(gen),
                distRadiance(gen),
                distRadiance(gen)
            );

            XMFLOAT3 position(
                distPos(gen),
                distPos(gen),
                distPos(gen)
            );

            std::cout << "Add light: "
                    << "Radiance(" << radiance.x << ", " << radiance.y << ", " << radiance.z << ") "
                    << "Position(" << position.x << ", " << position.y << ", " << position.z << ")"
                    << " Id (" << indexLight << ")" << std::endl;

            Light light(radiance, position);

            m_scene->addLight(light, { indexLight });
        }

        if(m_keys['F'])
        {
            constexpr uint32_t GOJO_INSTANCE_ID = 1;

            const Instance& instance = m_scene->getInstance({ GOJO_INSTANCE_ID });
            Instance movedGojo(instance);

            float x = distPos01(gen);
            float y = XMVectorGetY(movedGojo.m_transform.r[3]);
            float z = distPos01(gen);

            movedGojo.m_transform.r[3] = XMVectorSet(x, y, z, 1.0f);

            m_scene->modifyInstance(movedGojo, { GOJO_INSTANCE_ID });
        }

        if(m_keys['V'])
        {
            constexpr uint32_t INSTANCE_MESH_ID = 3;
            static uint32_t newInstanceId = 20;

            if(elapsedSeconds == 0)
            {
                Instance newInstance;
                newInstance.m_geometryReference.m_type = GeometryType::TRIANGLES;
                newInstance.m_geometryReference.m_geometryIndex = INSTANCE_MESH_ID;
                newInstance.m_transform = XMMatrixIdentity();
                float x = distPos01(gen);
                float y = 0;
                float z = distPos01(gen);
                newInstance.m_transform.r[3] = XMVectorSet(x, y, z, 1.0f);

                std::cout << "Add instance: " << newInstanceId << std::endl;

                m_scene->addInstance(newInstance, { newInstanceId++ });
            }
        }

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
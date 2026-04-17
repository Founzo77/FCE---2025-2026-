#include <windows.h>

#include <iostream>
#include <cstdio>

#include <fge/MainWindow.hpp>
#include <fge/io/GlobalLogger.hpp>

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    AllocConsole();
    FILE* fp;
    
    freopen_s(&fp, "CONOUT$", "w", stdout);
    //freopen_s(&fp, "output.txt", "w", stdout); freopen_s(&fp, "output.txt", "w", stderr);

    fge::enableAnsiColors();
    fge::MainWindow mainWindow(hInstance, 1000, 1000, "scene/shinjuku_gojo.xml");
    //fge::MainWindow mainWindow(hInstance, 1000, 1000, "scene/prototype.xml");
    mainWindow.initialize();
    mainWindow.run();

    Sleep(5000);
    FreeConsole();

    return 0;
}

#include <windows.h>

#include <iostream>
#include <cstdio>

#include <fce/MainWindow.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <streambuf>
#include <vector>
#include <fstream>

class multi_streambuf : public std::streambuf
{
public:
    multi_streambuf(std::initializer_list<std::streambuf*> buffers)
        : m_buffers(buffers) {}

protected:
    virtual int overflow(int c) override
    {
        for (auto buf : m_buffers)
            buf->sputc(c);
        return c;
    }

    virtual int sync() override
    {
        for (auto buf : m_buffers)
            buf->pubsync();
        return 0;
    }

private:
    std::vector<std::streambuf*> m_buffers;
};

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    //AllocConsole();
    //FILE* fp;
    
    //freopen_s(&fp, "CONOUT$", "w", stdout);
    //freopen_s(&fp, "output.txt", "w", stdout); freopen_s(&fp, "output.txt", "w", stderr);

    AllocConsole();

    // Ouvre la console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    // Ouvre le fichier
    std::ofstream file("output.txt");

    // Redirige std::cout vers console + fichier
    static multi_streambuf multiBuf({ std::cout.rdbuf(), file.rdbuf() });
    std::ostream multiOut(&multiBuf);

    // Remplace std::cout si tu veux
    std::cout.rdbuf(&multiBuf);

    fge::enableAnsiColors();
    fce::MainWindow mainWindow(hInstance, 1500, 1500, "scene/prototype.xml");
    mainWindow.initialize();
    mainWindow.run();

    Sleep(5000);
    FreeConsole();

    return 0;
}

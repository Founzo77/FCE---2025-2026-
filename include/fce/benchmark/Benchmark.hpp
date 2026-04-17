#pragma once

#include "BenchmarkApplication.hpp"
#include "BenchmarkConfig.hpp"

#include <fge/PlatformWindows.hpp>

#include <string>
#include <chrono>

using std::string;

namespace fce
{
    class Benchmark
    {
    private:
        HINSTANCE m_hInstance;
        HWND m_hWnd;
        uint32_t m_width;
        uint32_t m_height;
        string m_pathFileScene;
        bool m_isInit;
        BenchmarkApplication m_application;
        BenchmarkConfig m_benchmarkConfig;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        bool m_isBenchmarkRunning = false;
        bool m_isMouseLocked = true;

    public:
        Benchmark(HINSTANCE hInstance, uint32_t width, uint32_t height,
            const string& pathFileScene, BenchmarkConfig benchmarkConfig) noexcept;
        ~Benchmark() = default;

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
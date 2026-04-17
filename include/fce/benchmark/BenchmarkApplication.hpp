#pragma once

#include "EventRecorder.hpp"
#include "BenchmarkConfig.hpp"

#include "../Application.hpp"

namespace fce
{
    class BenchmarkApplication : protected Application
    {
    private:
        BenchmarkConfig m_config;
        EventRecorder m_eventRecorder;

    public:
        BenchmarkApplication() = default;
        ~BenchmarkApplication() = default;

        void initialize(HWND hWnd, uint32_t width, uint32_t height, const string& pathFileScene,
            const BenchmarkConfig& config);
        void update();
        void stopBenchmark();
    };
}
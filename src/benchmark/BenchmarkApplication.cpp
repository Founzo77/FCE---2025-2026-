#include <fce/benchmark/BenchmarkApplication.hpp>
#include <fce/benchmark/ExporterEvents.hpp>

namespace fce
{
    void BenchmarkApplication::initialize(HWND hWnd, 
        uint32_t width, uint32_t height, const string& pathFileScene, const BenchmarkConfig& config)
    {
        Application::initialize(hWnd, width, height, pathFileScene);
    }

    void BenchmarkApplication::update()
    {
        m_eventRecorder.startEvent("frame");
        Application::update();
        m_eventRecorder.endEvent("frame");
    }

    void BenchmarkApplication::stopBenchmark()
    {
        Application::stopEngine();
        ExporterEvents exporter;
        exporter.initialize(m_config);
        exporter.exportEvents(m_eventRecorder);
    }
}
#include <fce/benchmark/ExporterEvents.hpp>
#include <fce/benchmark/EventRecorder.hpp>

#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fstream>
#include <algorithm>
#include <numeric>

using fge::globalLogger;
using fge::throwIfFailed;

namespace fce
{
    void ExporterEvents::initialize(const BenchmarkConfig& config)
    {
        m_config = config;
    }

    void ExporterEvents::exportEvents(const EventRecorder& recorder)
    {
        std::ofstream file(m_config.m_outputFile);

        throwIfFailed(file.is_open(), "Failed to open export file");

        // Header CSV
        file << "Event,Count,Avg(ms),Min(ms),Max(ms),P95(ms),P99(ms)\n";

        const std::unordered_map<std::string, fce::EventData>& allEvents = 
            recorder.getAllEvents();

        for (const auto& [name, data] : allEvents)
        {
            std::vector<double> durations;

            for (const Event& event : data.m_events)
            {
                if (event.m_duration >= 0.0)
                    durations.push_back(event.m_duration);
            }

            if (durations.empty())
                continue;

            const size_t count = durations.size();

            const double sum = std::accumulate(durations.begin(), durations.end(), 0.0);

            const double avg = sum / static_cast<double>(count);

            const auto [minIt, maxIt] = std::minmax_element(durations.begin(), durations.end());

            const double min = *minIt;
            const double max = *maxIt;

            double p95 = computePercentile(durations, 0.95);
            double p99 = computePercentile(durations, 0.99);

            file << name << "," << count << "," << avg << "," << min << ","
                << max << "," << p95 << "," << p99 << "\n";
        }

        file.close();

        globalLogger().info() << "Benchmark exported to " << m_config.m_outputFile;
    }

    double ExporterEvents::computePercentile(const vector<double>& values, double percentile) const
    {
        if (values.empty())
            return 0.0;

        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());

        size_t index = static_cast<size_t>(percentile * (sorted.size() - 1));

        return sorted[index];
    }
}
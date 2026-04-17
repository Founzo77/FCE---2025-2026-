#pragma once

#include "BenchmarkConfig.hpp"

#include <vector>

using std::vector;

namespace fce
{
    class EventRecorder;

    class ExporterEvents
    {
    private:
        BenchmarkConfig m_config;

    public:
        ExporterEvents() = default;
        ~ExporterEvents() = default;

        ExporterEvents(const ExporterEvents&) = default;
        ExporterEvents(ExporterEvents&&) = default;

        ExporterEvents& operator=(const ExporterEvents&) = default;
        ExporterEvents& operator=(ExporterEvents&&) = default;

        void initialize(const BenchmarkConfig& config);
        void exportEvents(const EventRecorder& recorder);

    private:
        double computePercentile(const vector<double>& values, 
            double percentile) const;
    };
}
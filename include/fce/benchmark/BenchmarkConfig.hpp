#pragma once

#include <string>

using std::string;

namespace fce
{
    struct BenchmarkConfig
    {
    public:
        double m_duration;
        string m_outputFile = "benchmark.csv";

        BenchmarkConfig() = default;
        ~BenchmarkConfig() = default;

        BenchmarkConfig(const BenchmarkConfig&) = default;
        BenchmarkConfig(BenchmarkConfig&&) = default;

        BenchmarkConfig& operator=(const BenchmarkConfig&) = default;
        BenchmarkConfig& operator=(BenchmarkConfig&&) = default;
    };
}
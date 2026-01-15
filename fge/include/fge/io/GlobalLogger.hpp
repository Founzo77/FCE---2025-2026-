#pragma once

#include "Logger.hpp"

namespace fge
{
    static inline Logger& globalLogger()
    {
        static Logger instance(LoggerLevel::TRACE, std::cout);

        return instance;
    }
}
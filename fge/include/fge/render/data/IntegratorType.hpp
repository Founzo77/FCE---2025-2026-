#pragma once

#include <cstdint>

namespace fge
{
    enum class IntegratorType : uint32_t
    {
        PHONG = 0,
        PATHTRACER = 1
    };
}
#pragma once

#include "../PlatformWindows.hpp"

namespace fge
{
    class EventFactory
    {
    public:
        static HANDLE buildEvent();
    };
}

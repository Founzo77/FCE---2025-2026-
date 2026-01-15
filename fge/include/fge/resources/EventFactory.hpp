#pragma once

#include <Windows.h>

namespace fge
{
    class EventFactory
    {
    public:
        static HANDLE buildEvent();
    };
}

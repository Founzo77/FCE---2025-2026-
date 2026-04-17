#pragma once

#include <vector>
#include <chrono>

using std::vector;

namespace fce
{
    struct Event
    {
    public:
        double m_duration;
        std::chrono::high_resolution_clock::time_point m_start;

        Event() = default;
        ~Event() = default;

        Event(const Event&) = default;
        Event(Event&&) = default;

        Event& operator=(const Event&) = default;
        Event& operator=(Event&&) = default;
    };

    struct EventData
    {
    public:
        vector<Event> m_events;
        
        EventData() = default;
        ~EventData() = default;

        EventData(const EventData&) = default;
        EventData(EventData&&) = default;

        EventData& operator=(const EventData&) = default;
        EventData& operator=(EventData&&) = default;
    };
}
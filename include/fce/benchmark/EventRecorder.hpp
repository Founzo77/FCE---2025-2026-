#pragma once

#include "EventData.hpp"

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace fce
{
    class EventRecorder
    {
    private:
        unordered_map<string, EventData> m_events;

    public:
        void initialize();
        const EventData& getEventData(const string& name) const;
        const unordered_map<string, EventData>& getAllEvents() const noexcept;
        void startEvent(const string& name);
        void endEvent(const string& name);
    };
}
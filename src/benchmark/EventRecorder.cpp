#include <fce/benchmark/EventRecorder.hpp>

#include <chrono>
#include <algorithm>

namespace fce
{
    void EventRecorder::initialize()
    {
        m_events.clear();
    }

    const EventData& EventRecorder::getEventData(const string& name) const
    {
        auto it = m_events.find(name);

        if (it != m_events.end())
        {
            return it->second;
        }

        return {};
    }

    const unordered_map<string, EventData>& EventRecorder::getAllEvents() const noexcept
    {
        return m_events;
    }

    void EventRecorder::startEvent(const string& name)
    {
        std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

        Event event;
        event.m_start = now;
        event.m_duration = -1.0;

        m_events[name].m_events.push_back(event);
    }

    void EventRecorder::endEvent(const string& name)
    {
        std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();

        auto it = m_events.find(name);
        if (it == m_events.end())
        {
            return; // aucun event existant
        }

        auto& events = it->second.m_events;

        // Trouver le dernier event actif (LIFO logique)
        for (auto rit = events.rbegin(); rit != events.rend(); ++rit)
        {
            if (rit->m_duration < 0.0)
            {
                auto duration =
                    std::chrono::duration<double, std::milli>(now - rit->m_start).count();

                rit->m_duration = duration;
                return;
            }
        }

        // Aucun event actif trouvé -> start/end mal apparié
    }
}
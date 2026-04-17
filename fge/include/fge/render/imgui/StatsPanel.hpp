#pragma once

#include "ImGuiPanel.hpp"

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace fge
{
    class StatsPanel : public ImGuiPanel
    {
    private:
        string m_title;
        vector<float> m_fpsHistory;
        size_t m_offset;
        float m_currentFps;
        float m_currentFrameMs;

    public:
        StatsPanel(const string& title, const uint32_t nbFps);
        virtual ~StatsPanel();

        StatsPanel(const StatsPanel&) = default;
        StatsPanel(StatsPanel&&) = default;

        StatsPanel& operator=(const StatsPanel&) = default;
        StatsPanel& operator=(StatsPanel&&) = default;

        virtual void reset() override;
        virtual void drawUI() override;
    };
}

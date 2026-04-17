#include <fge/render/imgui/StatsPanel.hpp>

#include <imgui.h>

#include <numeric>

namespace fge
{
    StatsPanel::StatsPanel(const string& title, const uint32_t nbFps) : m_title(title),
        m_fpsHistory(nbFps), m_offset(0), m_currentFps(0.0f), m_currentFrameMs(0.0f)

    {

    }

    StatsPanel::~StatsPanel()
    {
        reset();
    }

    void StatsPanel::reset()
    {
        std::fill(m_fpsHistory.begin(), m_fpsHistory.end(), 0.0f);
        m_offset = 0;
        m_currentFps = 0.0f;
        m_currentFrameMs = 0.0f;
    }

    void StatsPanel::drawUI()
    {
        ImGuiIO& io = ImGui::GetIO();

        float deltaTime = io.DeltaTime;

        if (deltaTime > 0.0f)
        {
            m_currentFps = 1.0f / deltaTime;
            m_currentFrameMs = deltaTime * 1000.0f;
        }

        m_fpsHistory[m_offset] = m_currentFps;
        m_offset = (m_offset + 1) % m_fpsHistory.size();

        float minFps = *std::min_element(m_fpsHistory.begin(), m_fpsHistory.end());
        float maxFps = *std::max_element(m_fpsHistory.begin(), m_fpsHistory.end());

        float avgFps = std::accumulate(m_fpsHistory.begin(), m_fpsHistory.end(), 0.0f)
            / static_cast<float>(m_fpsHistory.size());

        ImGui::Begin(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("FPS: %.1f", m_currentFps);
        ImGui::Text("Frame: %.2f ms", m_currentFrameMs);
        ImGui::Separator();

        ImGui::Text("Avg: %.1f | Min: %.1f | Max: %.1f", avgFps, minFps, maxFps);

        float minMs = *std::min_element(m_fpsHistory.begin(), m_fpsHistory.end());
        float maxMs = *std::max_element(m_fpsHistory.begin(), m_fpsHistory.end());

        ImGui::PlotLines(
            "Frame Time (ms)",
            m_fpsHistory.data(),
            m_fpsHistory.size(),
            m_offset,
            nullptr,
            minMs - 1.0f,
            maxMs + 1.0f,
            ImVec2(0, 80)
        );

        ImGui::End();
    }
}

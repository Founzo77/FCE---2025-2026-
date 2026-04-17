#pragma once

namespace fge
{
    class ImGuiPanel
    {
    public:
        ImGuiPanel() = default;
        virtual ~ImGuiPanel() = default;

        ImGuiPanel(const ImGuiPanel&) = default;
        ImGuiPanel(ImGuiPanel&&) = default;

        ImGuiPanel& operator=(const ImGuiPanel&) = default;
        ImGuiPanel& operator=(ImGuiPanel&&) = default;

        virtual void reset() = 0;
        virtual void drawUI() = 0;
    };
}
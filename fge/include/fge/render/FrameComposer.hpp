#pragma once

#include "imgui/ImGuiLayer.hpp"

#include "../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace fge
{
    class FrameExecutionContext;

    class FrameComposer
    {
    private:
        ComPtr<ID3D12Device5> m_device;
        ImGuiLayer m_imGuiLayer;
        uint32_t m_width;
        uint32_t m_height;

    public:
        FrameComposer() = default;
        ~FrameComposer();

        void initialize(HWND hWnd, ComPtr<ID3D12Device5> device, 
            const uint32_t nbBackBuffers, const uint32_t width, const uint32_t height);
        void reset();
        void resize(const uint32_t width, const uint32_t height);
        void renderFrame(FrameExecutionContext& context);
    };
}
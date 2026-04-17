#pragma once

#include "ImGuiPanel.hpp"

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <cstdint>
#include <memory>

using namespace Microsoft::WRL;

using std::unique_ptr;

namespace fge
{
    class ImGuiLayer
    {
    private:
        ComPtr<ID3D12Device5> m_device;
        ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;

        D3D12_CPU_DESCRIPTOR_HANDLE m_fontCpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE m_fontGpuHandle{};

        uint32_t m_nbBufferingFrames = 0;
        DXGI_FORMAT m_backbufferFormat = DXGI_FORMAT_UNKNOWN;

        unique_ptr<ImGuiPanel> m_panel;

    public:
        ImGuiLayer() = default;
        ~ImGuiLayer();

        void initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
            uint32_t nbBufferingFrames, DXGI_FORMAT backbufferFormat);
        
        void reset();

        void beginFrame();
        void drawUI();
        void render(ID3D12GraphicsCommandList4* cmdList);
    };
}
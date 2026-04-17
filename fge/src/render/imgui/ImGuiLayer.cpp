#include <fge/render/imgui/ImGuiLayer.hpp>
#include <fge/render/imgui/StatsPanel.hpp>
#include <fge/render/imgui/ImGuiPanel.hpp>

#include <imgui.h>
#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_win32.h>

#include <fge/utility.hpp>

namespace fge
{
    ImGuiLayer::~ImGuiLayer()
    {
        reset();
    }

    void ImGuiLayer::initialize(HWND hWnd, ComPtr<ID3D12Device5> device,
        uint32_t nbBufferingFrames, DXGI_FORMAT backbufferFormat)
    {
        m_device = device;
        m_nbBufferingFrames = nbBufferingFrames;
        m_backbufferFormat = backbufferFormat;

        ImGui_ImplWin32_EnableDpiAwareness();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        ImFontConfig cfg;
        cfg.SizePixels = 18.0f;
        io.Fonts->AddFontDefaultVector(&cfg);

        ImGui::StyleColorsDark();

        float dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(hWnd);
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(dpiScale);

        // Descriptor heap (ImGui DOIT avoir un heap shader-visible)
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.NumDescriptors = m_nbBufferingFrames + 1;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        throwIfFailed(m_device->CreateDescriptorHeap(&heapDesc, 
            IID_PPV_ARGS(&m_descriptorHeap)), "Failed to create ImGui descriptor heap");

        m_fontCpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
        m_fontGpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();

        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX12_Init(m_device.Get(), m_nbBufferingFrames, m_backbufferFormat,
            m_descriptorHeap.Get(), m_fontCpuHandle, m_fontGpuHandle);

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        m_panel = std::make_unique<StatsPanel>("FGE", 1000);
    }

    void ImGuiLayer::reset()
    {
        if (ImGui::GetCurrentContext() != nullptr)
        {
            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }

        if(m_panel)
            m_panel->reset();
        m_panel.reset();

        m_fontCpuHandle = {};
        m_fontGpuHandle = {};

        m_nbBufferingFrames = 0;
        m_backbufferFormat = DXGI_FORMAT_UNKNOWN;

        m_descriptorHeap.Reset();
        m_device.Reset();
    }

    void ImGuiLayer::beginFrame()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::drawUI()
    {
        m_panel->drawUI();
    }

    void ImGuiLayer::render(ID3D12GraphicsCommandList4* cmdList)
    {
        ImGui::Render();

        ID3D12DescriptorHeap* heaps[] = { m_descriptorHeap.Get() };
        cmdList->SetDescriptorHeaps(1, heaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
    }
}
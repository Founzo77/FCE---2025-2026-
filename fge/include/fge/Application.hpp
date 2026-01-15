#pragma once

#include "FgeApplication.hpp"
#include "render/MainRenderer.hpp"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <cstdint>
#include <string>
#include <memory>

using namespace Microsoft::WRL;
using std::string;
using std::shared_ptr;

namespace fge
{
    class Scene;
    class SceneDescription;

    class Application : public FgeApplication
    {
    private:
        MainRenderer m_renderer;
        ComPtr<IDXGIAdapter4> m_adapter;
        ComPtr<ID3D12Device5> m_device;
        uint32_t m_nbFrames;
        bool m_isTearingSupport;
        bool m_isVSync;

    public:
        virtual ~Application() = default;

        void initializeSystem();
        void initializeMainRenderer(HWND hWnd, uint32_t width, uint32_t height, 
            SceneDescription& sceneDescription);
        void update();
        void stopEngine();

        shared_ptr<FgeScene> getScene();
        void resize(const uint32_t width, const uint32_t height);
        
        shared_ptr<Scene> getDirectScene();
        MainRenderer& getRender();

    private:
        void initializeDebugLayer();
        void initializeDirectXResources();
    };
}
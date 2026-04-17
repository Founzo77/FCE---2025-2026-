#pragma once

#include "../fge/PlatformWindows.hpp"

#include "render/MainRenderer.hpp"
#include "render/data/Library.hpp"
#include "render/data/Device.hpp"
#include "render/data/ImplementationCapability.hpp"

#include <fge/FgeApplication.hpp>
#include <fge/io/SceneDescription.hpp>

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include <memory>
using namespace Microsoft::WRL;

using std::shared_ptr;

namespace fge
{
    class FgeScene;
}

namespace fgewa
{
    class AnariRenderConfig;

    class AnariApplication : public fge::FgeApplication
    {
    private:
        MainRenderer m_renderer;
        Library m_library;
        shared_ptr<Device> m_device;
        shared_ptr<ImplementationCapability> m_capability;

        ComPtr<IDXGIAdapter4> m_dxrAdapter;
        ComPtr<ID3D12Device5> m_dxrDevice;
        uint32_t m_nbFrames;
        bool m_isTearingSupport;
        bool m_isVSync;

    public:
        AnariApplication() = default;
        virtual ~AnariApplication() = default;

        shared_ptr<Device> getDevice();
        shared_ptr<ImplementationCapability> getCapability();

        void initializeSystem() override;
        void initializeMainRenderer(HWND hWnd, uint32_t width, uint32_t height, 
            fge::SceneDescription& sceneDescription) override;
        void update() override;
        void stopEngine() override;

        shared_ptr<fge::FgeScene> getScene() override;
        void resize(const uint32_t width, const uint32_t height) override;

    private:
        void initializeAnariResources(const AnariRenderConfig& renderConfig);

        void initializeDebugLayer();
        void initializeDirectXResources();

    public:
        static void anariCallback(const void *userData, ANARIDevice device, ANARIObject source,
            ANARIDataType sourceType, ANARIStatusSeverity severity, ANARIStatusCode code,
            const char *message);
    };
}

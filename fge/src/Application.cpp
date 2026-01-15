#include <fge/Application.hpp>

#include <fge/render/MainRenderer.hpp>

#include <fge/utility.hpp>

#include <fge/resources/AdapterFactory.hpp>
#include <fge/resources/DirectXDeviceFactory.hpp>

#include <fge/io/GlobalLogger.hpp>

#include <dxgidebug.h>
#include <iostream>

namespace fge
{
    void Application::initializeSystem()
    {
        globalLogger().info() << "System initialization in progress";

        initializeDebugLayer();
        initializeDirectXResources();

        globalLogger().info() << "System initialized";
    }

    void Application::initializeMainRenderer(HWND hWnd, uint32_t width, uint32_t height, 
        SceneDescription& sceneDescription)
    {
        globalLogger().info() << "Main Renderer initialization in progress";

        m_renderer.initialize(hWnd, m_device, width, height, sceneDescription, m_nbFrames);

        globalLogger().info() << "Main Renderer initialized";
    }

    void Application::update()
    {
        m_renderer.renderFrame();
    }

    void Application::stopEngine()
    {
        globalLogger().info() << "Application shutdown in progress...";

        /*
        TO_DO
        #if defined(_DEBUG)
            ComPtr<IDXGIDebug1> dxgiDebug;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
            {
                globalLogger().info() << "=== DXGI Live Objects Report ===";
                dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
            }

            ComPtr<ID3D12DebugDevice2> debugDevice;
            if (SUCCEEDED(m_device.As(&debugDevice)))
            {
                globalLogger().info() << "=== D3D12 Live Device Objects ===";
                debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
            }
        #endif
        
        m_device.Reset();
        m_adapter.Reset();
        */

        ComPtr<IDXGIDebug1> dxgi_debug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgi_debug.GetAddressOf()))))
        {
            dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
        
        globalLogger().info() << "Application shutdown complete.";
    }

    shared_ptr<FgeScene> Application::getScene()
    {
        return std::static_pointer_cast<FgeScene>(m_renderer.getScene());
    }

    void Application::resize(const uint32_t width, const uint32_t height)
    {
        m_renderer.resize(width, height);
    }

    shared_ptr<Scene> fge::Application::getDirectScene()
    {
        return m_renderer.getScene();
    }

    MainRenderer& Application::getRender()
    {
        return m_renderer;
    }

    void Application::initializeDebugLayer()
    {
        #if defined(_DEBUG)
            globalLogger().info() << "Application en mode :'DEBUG'";
            ComPtr<ID3D12Debug> debugInterface;
            throwIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
            debugInterface->EnableDebugLayer();

            ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> dredSettings;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings))))
            {
                dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
                dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
                dredSettings->SetWatsonDumpEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

                globalLogger().info() << 
                    "[DRED] Activé (AutoBreadcrumbs + PageFault + WatsonDump)";
            }
            else
            {
                globalLogger().info() << "[DRED] Non disponible sur cette version du SDK";
            }
        #endif
    }

    void Application::initializeDirectXResources()
    {
        m_adapter = AdapterFactory::buildAdapterV4();
        m_device = DirectXDeviceFactory::buildDirectXDeviceV5(m_adapter);
        m_nbFrames = 3;
        // TO_DO
        m_isTearingSupport = true;
        m_isVSync = true;
    }
}

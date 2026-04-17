#include <fgewa/AnariApplication.hpp>
#include <fgewa/render/AnariScene.hpp>
#include <fgewa/render/AnariRenderConfig.hpp>

#include <fge/resources/AdapterFactory.hpp>
#include <fge/resources/DirectXDeviceFactory.hpp>

#include <fge/utility.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::throwIfFailed;
using fge::globalLogger;

namespace fgewa
{
    shared_ptr<Device> AnariApplication::getDevice()
    {
        return m_device;
    }

    shared_ptr<ImplementationCapability> AnariApplication::getCapability()
    {
        return m_capability;
    }

    void AnariApplication::initializeSystem()
    {
        globalLogger().info() << "System initialization in progress";

        initializeDebugLayer();
        initializeDirectXResources();

        globalLogger().info() << "System initialized";
    }

    void AnariApplication::initializeMainRenderer(HWND hWnd, uint32_t width, uint32_t height, 
        fge::SceneDescription& sceneDescription)
    {
        globalLogger().info() << "Main Renderer initialization in progress";

        AnariRenderConfig& renderConfig = 
            std::get<AnariRenderConfig>(sceneDescription.m_renderConfig);
        initializeAnariResources(renderConfig);
        m_capability = std::make_shared<ImplementationCapability>(
            ImplementationCapability::fromLibraryName(renderConfig.m_libraryName));
        m_renderer.initialize(m_device, m_capability, 
            width, height, sceneDescription, hWnd, m_dxrDevice, m_nbFrames);

        globalLogger().info() << "Main Renderer initialized";
    }

    void AnariApplication::update()
    {
        m_renderer.renderFrame();
    }

    void AnariApplication::stopEngine()
    {
        //m_renderer = MainRenderer;
        m_device.reset();
        m_library.reset();
    }

    shared_ptr<fge::FgeScene> AnariApplication::getScene()
    {
        return std::static_pointer_cast<fge::FgeScene>(m_renderer.getScene());
    }

    void AnariApplication::resize(const uint32_t width, const uint32_t height)
    {
        m_renderer.resize(width, height);
    }

    void AnariApplication::initializeAnariResources(const AnariRenderConfig& renderConfig)
    {
        m_library.initialize(renderConfig.m_libraryName.c_str(), &AnariApplication::anariCallback);

        m_device = std::make_shared<Device>();
        m_device->initialize(m_library, &AnariApplication::anariCallback);

        globalLogger().info() 
            << "Anari implementation used: " << renderConfig.m_libraryName.c_str();
    }

    void AnariApplication::initializeDebugLayer()
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

    void AnariApplication::initializeDirectXResources()
    {
        m_dxrAdapter = fge::AdapterFactory::buildAdapterV4();
        m_dxrDevice = fge::DirectXDeviceFactory::buildDirectXDeviceV5(m_dxrAdapter);
        m_nbFrames = 3;
        // TO_DO
        m_isTearingSupport = true;
        m_isVSync = true;
    }

    void AnariApplication::anariCallback(const void* /*userData*/, ANARIDevice /*device*/,
        ANARIObject /*source*/, ANARIDataType /*sourceType*/, ANARIStatusSeverity severity,
        ANARIStatusCode /*code*/, const char* message)
    {
        switch (severity)
        {
        case ANARI_SEVERITY_FATAL_ERROR:
            globalLogger().critical() << "[ANARI] " << message;
            break;

        case ANARI_SEVERITY_ERROR:
            globalLogger().error() << "[ANARI] " << message;
            break;

        case ANARI_SEVERITY_WARNING:
            globalLogger().warning() << "[ANARI] " << message;
            throwIfFailed(0 == 1, "We don't want that");
            break;

        case ANARI_SEVERITY_PERFORMANCE_WARNING:
            //globalLogger().warning() << "[ANARI][PERF] " << message;
            //throwIfFailed(0 == 1, "We don't want that");
            break;

        case ANARI_SEVERITY_INFO:
            globalLogger().info() << "[ANARI] " << message;
            break;

        case ANARI_SEVERITY_DEBUG:
            //globalLogger().debug() << "[ANARI] " << message;
            break;

        default:
            globalLogger().trace() << "[ANARI] " << message;
            break;
        }
    }


}
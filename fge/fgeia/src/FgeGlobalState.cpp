#include <fgeia/FgeGlobalState.hpp>

#include <fge/utility.hpp>
#include <fge/resources/AdapterFactory.hpp>
#include <fge/resources/DirectXDeviceFactory.hpp>
#include <fge/io/GlobalLogger.hpp>

using fge::globalLogger;
using fge::throwIfFailed;

namespace fgeia
{
    FgeGlobalState::FgeGlobalState(ANARIDevice device) : helium::BaseGlobalDeviceState(device),
        m_isRenderEngineInitialized(false)
    {
        initializeDebugLayer();
        initializeDirectXResources();

        // TO_DO Mettre les bon nombre max d'elements par rapport au moteur fge

        m_lightIndexAllocator.initialize(1000);
        m_meshIndexAllocator.initialize(1000);
        m_volumeIndexAllocator.initialize(1000);
        m_instanceIndexAllocator.initialize(1000);
        m_textureIndexAllocator.initialize(1000);
        m_texture3DIndexAllocator.initialize(1000);
        m_materialIndexAllocator.initialize(1000);
    }

    FgeGlobalState::~FgeGlobalState()
    {
        reset();
    }

    void FgeGlobalState::reset()
    {
        m_fgeScene.reset();
        m_renderEngine.reset();
        m_device.Reset();
        m_adapter.Reset();
    }

    void FgeGlobalState::initializeDebugLayer()
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

    void FgeGlobalState::initializeDirectXResources()
    {
        m_adapter = fge::AdapterFactory::buildAdapterV4();
        d12SetDebugName(m_adapter, L"FGE Adaptater");
        m_device = fge::DirectXDeviceFactory::buildDirectXDeviceV5(m_adapter);
        d12SetDebugName(m_device, L"FGE Device");
    }

    fge::RenderEngine& FgeGlobalState::getRenderEngine() noexcept
    {
        return m_renderEngine;
    }

    const fge::RenderEngine& FgeGlobalState::getRenderEngine() const noexcept
    {
        return m_renderEngine;
    }

    fge::SceneDescription& FgeGlobalState::getSceneDescription() noexcept
    {
        return m_sceneDescription;
    }

    const fge::SceneDescription& FgeGlobalState::getSceneDescription() const noexcept
    {
        return m_sceneDescription;
    }

    ComPtr<ID3D12Device5> FgeGlobalState::getDevice() noexcept
    {
        return m_device;
    }

    fge::RenderEngine& FgeGlobalState::startRender(
        const uint32_t width, const uint32_t height)
    {
        if(m_isRenderEngineInitialized == false)
        {
            m_renderEngine.initialize(getDevice(), width, height, getSceneDescription(), 3);
            m_fgeScene = m_renderEngine.getScene();
            m_isRenderEngineInitialized = true;
        }
        
        return m_renderEngine;
    }

    void FgeGlobalState::setCamera(const fge::Camera& fgeCamera)
    {
        if(m_isRenderEngineInitialized == false)
        {
            m_sceneDescription.m_camera = fgeCamera;
        }
        else
        {
            m_fgeScene->setCamera(fgeCamera);
        }
    }

    void FgeGlobalState::setLight(const fge::Light& fgeLight, const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            if(m_sceneDescription.m_lights.size() <= index.m_index)
                m_sceneDescription.m_lights.resize(index.m_index + 1);
            m_sceneDescription.m_lights[index.m_index] = fgeLight;
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setMesh(fge::Mesh&& fgeMesh, const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            m_sceneDescription.m_meshes[index.m_index] = std::move(fgeMesh);
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setVolume(fge::Volume&& fgeVolume, const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            m_sceneDescription.m_volumes[index.m_index] = std::move(fgeVolume);
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setInstance(const fge::Instance& fgeInstance, 
        const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            m_sceneDescription.m_instances[index.m_index] = fgeInstance;
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setTexture2D(fge::Texture&& fgeTexture2D, 
        const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            if(m_sceneDescription.m_textures.size() <= index.m_index)
                m_sceneDescription.m_textures.resize(index.m_index + 1);
            m_sceneDescription.m_textures[index.m_index] = std::move(fgeTexture2D);
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setTexture3D(fge::Texture&& fgeTexture3D, 
        const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            if(m_sceneDescription.m_textures3D.size() <= index.m_index)
                m_sceneDescription.m_textures3D.resize(index.m_index + 1);
            m_sceneDescription.m_textures3D[index.m_index] = std::move(fgeTexture3D);
        }
        else
        {
            // TO_DO
        }
    }

    void FgeGlobalState::setMaterial(const fge::Material& fgeMaterial, 
        const fge::LogicalIndex index)
    {
        if(m_isRenderEngineInitialized == false)
        {
            if(m_sceneDescription.m_materials.size() <= index.m_index)
                m_sceneDescription.m_materials.resize(index.m_index + 1);
            m_sceneDescription.m_materials[index.m_index] = fgeMaterial;
        }
        else
        {
            // TO_DO
        }
    }
}
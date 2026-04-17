#pragma once

#include <helium/BaseGlobalDeviceState.h>

#include <fge/render/RenderEngine.hpp>
#include <fge/io/SceneDescription.hpp>
#include <fge/memory/IndexAllocator.hpp>

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

using namespace Microsoft::WRL;

namespace fgeia
{
    class FgeGlobalState : public helium::BaseGlobalDeviceState
    {
    private:
        ComPtr<IDXGIAdapter4> m_adapter;
        ComPtr<ID3D12Device5> m_device;
        fge::RenderEngine m_renderEngine;
        shared_ptr<fge::Scene> m_fgeScene;
        bool m_isRenderEngineInitialized;
    
    public:
        fge::SceneDescription m_sceneDescription;
    
        fge::CompactIndexAllocator m_lightIndexAllocator;
        fge::CompactIndexAllocator m_meshIndexAllocator;
        fge::CompactIndexAllocator m_volumeIndexAllocator;
        fge::CompactIndexAllocator m_instanceIndexAllocator;
        fge::CompactIndexAllocator m_textureIndexAllocator;
        fge::CompactIndexAllocator m_texture3DIndexAllocator;
        fge::CompactIndexAllocator m_materialIndexAllocator;

    public:
        FgeGlobalState(ANARIDevice device);
        ~FgeGlobalState();

        void reset();

    private:
        void initializeDebugLayer();
        void initializeDirectXResources();

    public:
        fge::RenderEngine& getRenderEngine() noexcept;
        const fge::RenderEngine& getRenderEngine() const noexcept;

        fge::SceneDescription& getSceneDescription() noexcept;
        const fge::SceneDescription& getSceneDescription() const noexcept;

        ComPtr<ID3D12Device5> getDevice() noexcept;

        fge::RenderEngine& startRender(const uint32_t width, const uint32_t height);

        void setCamera(const fge::Camera& fgeCamera);
        void setLight(const fge::Light& fgeLight, const fge::LogicalIndex index);
        void setMesh(fge::Mesh&& fgeMesh, const fge::LogicalIndex index);
        void setVolume(fge::Volume&& fgeVolume, const fge::LogicalIndex index);
        void setInstance(const fge::Instance& fgeInstance, const fge::LogicalIndex index);
        void setTexture2D(fge::Texture&& fgeTexture2D, const fge::LogicalIndex index);
        void setTexture3D(fge::Texture&& fgeTexture3D, const fge::LogicalIndex index);
        void setMaterial(const fge::Material& fgeMaterial, const fge::LogicalIndex index);
    };
}

#include <fge/render/RenderEngine.hpp>

#include <fge/resources/FenceFactory.hpp>
#include <fge/resources/EventFactory.hpp>

#include <fge/utility.hpp>
#include <fge/render/utility.hpp>

namespace fge
{
    RenderEngine::~RenderEngine()
    {
        reset();
    }

    void RenderEngine::initialize(ComPtr<ID3D12Device5> device, const uint32_t width,
        const uint32_t height, SceneDescription& sceneDescription,
        const uint32_t nbFrameContexts)
    {
        m_device = device;
        
        initializeDeviceResources(width, height, nbFrameContexts);
        initializeScene(sceneDescription);

        throwIfFailed(m_frameContexts[m_currentIdFrame].m_directCommandList->Close(),
            "Failed to close m_directCommandList");

        ID3D12CommandList* const commandLists[] = {
            m_frameContexts[m_currentIdFrame].m_directCommandList.Get()
        };
        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);

        signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderFenceValue);
        m_frameContexts[m_currentIdFrame].m_fenceValue = m_renderFenceValue;

        // TO_DO Regarder si synchro peut se faire apres m_rayTracingPipeline.initialize(...)
        waitForFenceValue(m_renderFence, m_renderFenceEvent, 
            m_frameContexts[m_currentIdFrame].m_fenceValue);
        
        m_rayTracingPipeline = std::make_shared<RayTracingPipeline>();
        m_rayTracingPipeline->initialize(device, m_scene->getMemory(), width, height);
        m_scene->getMemory()->setRayTracingPipeline(m_rayTracingPipeline);
    }

    void RenderEngine::initializeDeviceResources(
        const uint32_t width, const uint32_t height, const uint32_t nbFrameContexts)
    {
        m_directCommandQueue.initialize(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);

        m_renderFence = FenceFactory::buildFence(m_device);
        d12SetDebugName(m_renderFence, L"ID3D12Fence");
        m_renderFenceEvent = EventFactory::buildEvent();
        m_renderFenceValue = 0;

        m_frameContexts.resize(nbFrameContexts);

        for(size_t i = 0; i < m_frameContexts.size(); i++)
        {
            m_frameContexts[i].initialize(m_device);
        }

        m_currentIdFrame = 0;
        m_frameContexts[m_currentIdFrame].resetCommand();
    }

    void RenderEngine::initializeScene(SceneDescription& sceneDescription)
    {
        m_scene = std::make_shared<Scene>();
        m_scene->buildScene(
            m_device, m_frameContexts[m_currentIdFrame].m_directCommandList, sceneDescription);
    }

    void RenderEngine::reset()
    {
        if(!m_device)
            return;
            
        if (m_directCommandQueue.m_commandQueue && m_renderFence && m_renderFenceEvent)
        {
            m_directCommandQueue.flush(m_renderFence, m_renderFenceEvent, m_renderFenceValue);
        }

        m_rayTracingPipeline->reset();
        m_rayTracingPipeline.reset();
        m_scene->reset();
        m_scene.reset();

        m_frameContexts.clear();
        m_currentIdFrame = 0;

        m_directCommandQueue.reset();

        CloseHandle(m_renderFenceEvent);
        m_renderFence.Reset();
        m_renderFenceValue = 0;

        m_device.Reset();
    }

    shared_ptr<Scene> RenderEngine::getScene()
    {
        return m_scene;
    }

    ID3D12Resource* RenderEngine::getOutputTexture()
    {
        return m_scene->getMemory()->getOutputTextureBufferResource();
    }

    HANDLE RenderEngine::getSharedOutputTextureHandle()
    {
        return m_scene->getMemory()->getSharedOutputTextureHandle();
    }

    ID3D12CommandQueue* RenderEngine::getDirectCommandQueue()
    {
        return m_directCommandQueue.m_commandQueue.Get();
    }

    void RenderEngine::resize(const uint32_t width, const uint32_t height)
    {
        m_directCommandQueue.flush(m_renderFence, m_renderFenceEvent, m_renderFenceValue);
        m_rayTracingPipeline->resize(width, height);
    }

    FrameExecutionContext RenderEngine::startRender()
    {
        m_frameContexts[m_currentIdFrame].resetCommand();

        m_scene->getMemory()->updateDevice(
            m_device, m_frameContexts[m_currentIdFrame].m_directCommandList,
            m_currentIdFrame);
        
        D3D12_RESOURCE_BARRIER uavBarriers[2] = {};

        uavBarriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        uavBarriers[0].UAV.pResource = m_scene->getMemory()->getAccumulatedLeftTextureBufferResource();

        uavBarriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        uavBarriers[1].UAV.pResource = m_scene->getMemory()->getAccumulatedRightTextureBufferResource();

        m_rayTracingPipeline->bindDescriptors(m_frameContexts[m_currentIdFrame].m_directCommandList,
            m_currentIdFrame);
        
        /*
        if(m_scene->getMemory()->m_currentDeviceConstantInformations.m_sppIndex == 0)
        {
            auto cmd = m_frameContexts[m_currentIdFrame].m_directCommandList.Get();

            float clearValue[4] = {0.f, 0.f, 0.f, 0.f};
            const std::shared_ptr<fge::SceneMemoryManager> memory = m_scene->getMemory();

            // left buffer
            cmd->ClearUnorderedAccessViewFloat(
                m_rayTracingPipeline->getHandleView().getAccumulatedLeftTextureUavHandle(),
                m_rayTracingPipeline->getHandleHostView().getAccumulatedLeftTextureHostUavHandle(),
                memory->getAccumulatedLeftTextureBufferResource(),
                clearValue, 0, nullptr);

            // right buffer
            cmd->ClearUnorderedAccessViewFloat(
                m_rayTracingPipeline->getHandleView().getAccumulatedRightTextureUavHandle(),
                m_rayTracingPipeline->getHandleHostView().getAccumulatedRightTextureHostUavHandle(),
                memory->getAccumulatedRightTextureBufferResource(),
                clearValue, 0, nullptr);

            m_frameContexts[m_currentIdFrame].m_directCommandList->ResourceBarrier(2, uavBarriers);
        }
        */

        m_rayTracingPipeline->dispatchRays(m_frameContexts[m_currentIdFrame].m_directCommandList);

        m_frameContexts[m_currentIdFrame].m_directCommandList->ResourceBarrier(2, uavBarriers);

        FrameExecutionContext executionContext;
        executionContext.m_commandList = 
            m_frameContexts[m_currentIdFrame].m_directCommandList.Get();
        executionContext.m_outputTexture = 
            m_scene->getMemory()->getOutputTextureBufferResource();

        return executionContext;
    }

    void RenderEngine::executeRender()
    {
        throwIfFailed(m_frameContexts[m_currentIdFrame].m_directCommandList->Close());

        ID3D12CommandList* const commandLists[] = {
            m_frameContexts[m_currentIdFrame].m_directCommandList.Get()
        };

        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);
    }

    void RenderEngine::copyOutputToHost()
    {
        ID3D12Resource* srcTexture = getOutputTexture();

        D3D12_RESOURCE_DESC desc = srcTexture->GetDesc();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        UINT numRows;
        UINT64 rowSize;
        UINT64 totalSize;

        m_device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &numRows, &rowSize, &totalSize);

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            srcTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
        m_frameContexts[m_currentIdFrame].m_directCommandList.Get()->ResourceBarrier(1, &barrier);

        D3D12_TEXTURE_COPY_LOCATION src = {};
        src.pResource = srcTexture;
        src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION dst = {};
        dst.pResource = m_scene->getMemory()->getHostOutputTextureBufferResource();
        dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        dst.PlacedFootprint = footprint;

        m_frameContexts[m_currentIdFrame].m_directCommandList.Get()->CopyTextureRegion(
            &dst, 0, 0, 0, &src, nullptr);

        std::swap(barrier.Transition.StateBefore, barrier.Transition.StateAfter);
        m_frameContexts[m_currentIdFrame].m_directCommandList.Get()->ResourceBarrier(1, &barrier);
    }

    void RenderEngine::copyOutputToSharedResource()
    {
        ID3D12Resource* srcTexture = getOutputTexture();
        ID3D12Resource* dstTexture = m_scene->getMemory()->getSharedOutputTextureBufferResource();

        auto cmdList = m_frameContexts[m_currentIdFrame].m_directCommandList.Get();

        // transition source -> COPY_SOURCE
        D3D12_RESOURCE_BARRIER srcBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            srcTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

        cmdList->ResourceBarrier(1, &srcBarrier);
        cmdList->CopyResource(dstTexture, srcTexture);

        std::swap(srcBarrier.Transition.StateBefore, srcBarrier.Transition.StateAfter);
        cmdList->ResourceBarrier(1, &srcBarrier);
    }

    void RenderEngine::endRender()
    {
        signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderFenceValue);
        m_frameContexts[m_currentIdFrame].m_fenceValue = m_renderFenceValue;

        m_currentIdFrame++;
        if(m_currentIdFrame >= m_frameContexts.size())
            m_currentIdFrame = 0;

        waitForFenceValue(m_renderFence, m_renderFenceEvent, 
            m_frameContexts[m_currentIdFrame].m_fenceValue);
    }

    void RenderEngine::synchronize()
    {
        signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderFenceValue);
        waitForFenceValue(m_renderFence, m_renderFenceEvent, m_renderFenceValue);
    }

    void RenderEngine::copyOutputHostToHost(void* dstBuffer, uint64_t dstSize)
    {
        ID3D12Resource* srcTexture = getOutputTexture();

        D3D12_RESOURCE_DESC desc = srcTexture->GetDesc();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        UINT numRows;
        UINT64 rowSize;
        UINT64 totalSize;

        m_device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &numRows, &rowSize, &totalSize);

        void* mapped = nullptr;
        D3D12_RANGE range = {0, totalSize};

        auto hostBuffer = m_scene->getMemory()->getHostOutputTextureBufferResource();

        throwIfFailed(hostBuffer->Map(0, &range, &mapped), "Failed to map host output buffer");

        uint8_t* src = reinterpret_cast<uint8_t*>(mapped);
        uint8_t* dst = reinterpret_cast<uint8_t*>(dstBuffer);

        uint32_t width = (uint32_t)desc.Width;
        uint32_t height = desc.Height;

        const uint32_t pixelSize = 4; // RGBA8
        const uint64_t srcPitch = footprint.Footprint.RowPitch;
        const uint64_t dstPitch = width * pixelSize;

        for (uint32_t y = 0; y < height; ++y)
        {
            for (uint32_t x = 0; x < width; ++x)
            {
                uint32_t srcX = width - 1 - x;
                uint32_t srcY = height - 1 - y;

                uint8_t* srcPixel = src + srcY * srcPitch + srcX * pixelSize;
                uint8_t* dstPixel = dst + y * dstPitch + x * pixelSize;

                memcpy(dstPixel, srcPixel, pixelSize);
            }
        }

        hostBuffer->Unmap(0, nullptr);
    }
}

#include <fgewa/render/MainRenderer.hpp>
#include <fgewa/render/AnariScene.hpp>
#include <fgewa/render/PresentHostFrame.hpp>
#include <fgewa/render/PresentCudaFrame.hpp>
#include <fgewa/render/PresentDirectX12Frame.hpp>
#include <fgewa/render/data/Device.hpp>
#include <fgewa/render/data/ImplementationCapability.hpp>

#include <fge/utility.hpp>
#include <fge/render/utility.hpp>
#include <fge/render/FrameExecutionContext.hpp>

#include <fge/resources/SwapChainFactory.hpp>
#include <fge/resources/FenceFactory.hpp>
#include <fge/resources/EventFactory.hpp>
#include <fge/resources/DescriptorHeapFactory.hpp>

#include <anari/anari.h>

#include <fstream>
#include <vector>
#include <algorithm>

using fge::throwIfFailed;

namespace fgewa
{
    void MainRenderer::initialize(shared_ptr<Device> device, 
        shared_ptr<ImplementationCapability> capability, const uint32_t width,
        const uint32_t height, fge::SceneDescription& sceneDescription,
        HWND hWnd, ComPtr<ID3D12Device5> dxrDevice, uint32_t nbFrames)
    {
        m_scene = std::make_shared<AnariScene>();
        m_scene->buildScene(device, sceneDescription, width, height);
        m_device = device;
        m_capability = capability;
        m_width = width;
        m_height = height;
        m_isFirstFrame = true;

        initializeDeviceResources(hWnd, dxrDevice, width, height, nbFrames);

        if(m_capability->hasOutputDirectX12Buffer())
            m_presentFrame = std::make_unique<PresentDirectX12Frame>();
        else if(m_capability->hasOutputCudaBuffer())
            m_presentFrame = std::make_unique<PresentCudaFrame>();
        else
            m_presentFrame = std::make_unique<PresentHostFrame>();

        m_presentFrame->initialize(device, hWnd, dxrDevice, 
            m_directCommandQueue.m_commandQueue.Get(), width, height, nbFrames);

        throwIfFailed(m_frames[m_currentIdFrame].m_directCommandList->Close(),
            "Failed to close m_directCommandList");

        ID3D12CommandList* const commandLists[] = { 
            m_frames[m_currentIdFrame].m_directCommandList.Get() };
        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);

         fge::signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderValue);
        m_frames[m_currentIdFrame].m_fenceValue = m_renderValue;

        fge::waitForFenceValue(
            m_renderFence, m_renderEvent, m_frames[m_currentIdFrame].m_fenceValue);
    }

    shared_ptr<AnariScene> MainRenderer::getScene()
    {
        return m_scene;
    }

    void MainRenderer::renderFrame()
    {
        fge::FrameExecutionContext executionContext;
        executionContext.m_commandList = m_frames[m_currentIdFrame].m_directCommandList.Get();
        m_frames[m_currentIdFrame].resetCommand();

        m_presentFrame->setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
        m_barriers.submit(executionContext.m_commandList);
        
        if(m_isFirstFrame == false)
        {
            throwIfFailed(anariFrameReady(m_device->getHandle(), m_scene->getFrame().getHandle(), 
                ANARI_WAIT) != 0, "Frame unfinished");
            m_presentFrame->copyOutputToRTV(executionContext.m_commandList, m_scene->getFrame());
        }

        m_presentFrame->setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_barriers.submit(executionContext.m_commandList);

        m_presentFrame->setOmSetRenderTarget(executionContext);
        m_frameComposer.renderFrame(executionContext);

        m_presentFrame->setBackBufferState(m_barriers,
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_barriers.submit(executionContext.m_commandList);

        throwIfFailed(executionContext.m_commandList->Close());

        ID3D12CommandList* const commandLists[] = {
            executionContext.m_commandList
        };

        m_directCommandQueue.m_commandQueue->ExecuteCommandLists(1, commandLists);

        m_presentFrame->presentFrame();

        fge::signal(m_directCommandQueue.m_commandQueue, m_renderFence, m_renderValue);
        m_frames[m_currentIdFrame].m_fenceValue = m_renderValue;

        m_currentIdFrame++;
        if(m_currentIdFrame >= m_frames.size())
            m_currentIdFrame = 0;

        fge::waitForFenceValue(m_renderFence, m_renderEvent, 
            m_frames[m_currentIdFrame].m_fenceValue);

        computeAnariFrame();
        m_isFirstFrame = false;
    }

    void MainRenderer::resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        m_frameComposer.resize(width, height);
        m_presentFrame->resize(width, height);
        // TO_DO
    }

    void MainRenderer::initializeDeviceResources(HWND hWnd, ComPtr<ID3D12Device5> device,
        uint32_t width, uint32_t height, uint32_t nbFrames)
    {
        m_dxrDevice = device;

        m_directCommandQueue.initialize(m_dxrDevice, D3D12_COMMAND_LIST_TYPE_DIRECT);

        m_renderFence = fge::FenceFactory::buildFence(m_dxrDevice);
        m_renderEvent = fge::EventFactory::buildEvent();
        m_renderValue = 0;

        const uint32_t NB_FRAMES = 2;
        m_frames.resize(NB_FRAMES);

        for(size_t i = 0; i < m_frames.size(); i++)
        {
            m_frames[i].initialize(device);
        }

        m_frameComposer.initialize(hWnd, device, NB_FRAMES, width, height);

        m_currentIdFrame = 0;
        m_frames[m_currentIdFrame].resetCommand();
    }

    void MainRenderer::computeAnariFrame()
    {
        anariRenderFrame(m_device->getHandle(), m_scene->getFrame().getHandle());
    }
}

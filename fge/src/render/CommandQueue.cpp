#include <fge/render/CommandQueue.hpp>
#include <fge/utility.hpp>
#include <fge/render/utility.hpp>
#include <fge/resources/CommandQueueFactory.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void CommandQueue::initialize(ComPtr<ID3D12Device5> device, 
        D3D12_COMMAND_LIST_TYPE type)
    {
        m_commandQueue = CommandQueueFactory::buildCommandQueue(device, type);
        d12SetDebugName(m_commandQueue);
    }

    void CommandQueue::reset()
    {
        m_commandQueue.Reset();
    }

    void fge::CommandQueue::flush(ComPtr<ID3D12Fence> fence, HANDLE event, 
        uint64_t& fenceValue)
    {
        signal(m_commandQueue, fence, fenceValue);
        waitForFenceValue(fence, event, fenceValue);
    }
}
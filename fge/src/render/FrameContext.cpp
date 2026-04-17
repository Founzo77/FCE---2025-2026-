#include <fge/render/FrameContext.hpp>
#include <fge/resources/CommandAllocatorFactory.hpp>
#include <fge/resources/CommandListFactory.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void FrameContext::initialize(ComPtr<ID3D12Device5> device)
    {
        m_directCommandAllocator = CommandAllocatorFactory::buildCommandAllocator(device,
            D3D12_COMMAND_LIST_TYPE_DIRECT);
        d12SetDebugName(m_directCommandAllocator, L"ID3D12CommandAllocator");

        m_directCommandList = CommandListFactory::buildCommandListV4(device,
            m_directCommandAllocator, D3D12_COMMAND_LIST_TYPE_DIRECT);
        d12SetDebugName(m_directCommandList, L"ID3D12GraphicsCommandList4");

        m_fenceValue = 0;

        //resetCommand();
    }

    void FrameContext::resetCommand()
    {
        m_directCommandAllocator->Reset();
        m_directCommandList->Reset(m_directCommandAllocator.Get(), nullptr);
    }
}
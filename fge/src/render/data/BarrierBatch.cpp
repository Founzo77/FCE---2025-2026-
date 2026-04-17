#include <fge/render/data/BarrierBatch.hpp>

namespace fge
{
    BarrierBatch::~BarrierBatch()
    {
        reset();
    }

    void BarrierBatch::reset()
    {
        m_barriers.clear();
        m_barriers.shrink_to_fit();
    }

    void BarrierBatch::add(const CD3DX12_RESOURCE_BARRIER barrier)
    {
        m_barriers.push_back(barrier);
    }

    void BarrierBatch::submit(ID3D12GraphicsCommandList4* commandList)
    {
        commandList->ResourceBarrier(m_barriers.size(), m_barriers.data());
        m_barriers.clear();
    }
}
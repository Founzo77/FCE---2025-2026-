#include <fge/render/FrameExecutionContext.hpp>

#include <fge/render/data/BarrierBatch.hpp>

namespace fge
{
    void FrameExecutionContext::setOutputTextureState(BarrierBatch& barrierBatch, 
        const D3D12_RESOURCE_STATES beforState, const D3D12_RESOURCE_STATES afterState)
    {
        barrierBatch.add(CD3DX12_RESOURCE_BARRIER::Transition(
            m_outputTexture, beforState, afterState));
    }
}
#pragma once

#include <wrl/client.h>
#include <d3d12.h>

using namespace Microsoft::WRL;

namespace fge
{
    class BarrierBatch;
    
    class FrameExecutionContext
    {
    public:
        ID3D12GraphicsCommandList4* m_commandList;
        ID3D12Resource* m_outputTexture;

        FrameExecutionContext() = default;
        ~FrameExecutionContext() = default;

        FrameExecutionContext(const FrameExecutionContext&) = default;
        FrameExecutionContext& operator=(const FrameExecutionContext&) = default;
        FrameExecutionContext(FrameExecutionContext&&) = default;
        FrameExecutionContext& operator=(FrameExecutionContext&&) = default;

        void setOutputTextureState(BarrierBatch& barrierBatch,
            const D3D12_RESOURCE_STATES beforState, const D3D12_RESOURCE_STATES afterState);
    };
}

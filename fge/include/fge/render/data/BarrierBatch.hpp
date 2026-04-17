#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class BarrierBatch
    {
    private:
        vector<CD3DX12_RESOURCE_BARRIER> m_barriers;

    public:
        BarrierBatch() = default;
        ~BarrierBatch();

        BarrierBatch(const BarrierBatch&) = default;
        BarrierBatch& operator=(const BarrierBatch&) = default;
        BarrierBatch(BarrierBatch&&) = default;
        BarrierBatch& operator=(BarrierBatch&&) = default;

        void reset();
        void add(const CD3DX12_RESOURCE_BARRIER barrier);
        void submit(ID3D12GraphicsCommandList4* commandList);
    };
}
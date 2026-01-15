#pragma once

#include "UploadBuffer.hpp"
#include "../indices.hpp"
#include "../../memory/CompactStackAllocator.hpp"
#include "../../memory/IndexRange.hpp"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>
#include <unordered_map>

using namespace Microsoft::WRL;
using std::vector;
using std::unordered_map;

namespace fge
{
    class Light;

    class LightMemoryManager
    {
    public:
        CompactStackAllocator m_lightsPool;
        unordered_map<uint32_t, uint32_t> m_logicalIndexToPhysical;
        vector<IndexRange> m_physicalLightsUpdated;

        ComPtr<ID3D12Heap> m_deviceHeap;
        ComPtr<ID3D12Resource> m_deviceBuffer;
        UploadBuffer m_uploadBuffer;
        bool m_hasLightsResized;

    public:
        LightMemoryManager() = default;
        ~LightMemoryManager() = default;

        void startInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void moveAllToDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        uint64_t getLightPageSize() const;
        uint64_t getNbMaxLights() const;

        uint32_t getNbLights() const;

        Light& get(const LogicalIndex logicalLightIndex);
        const Light& get(const LogicalIndex logicalLightIndex) const;

        CompactStackAllocator::OccupiedElementRange<Light> iterateOverLights();

        void updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        PhysicalIndex add(const Light& light, const LogicalIndex logicalLightIndex);
        void free(const LogicalIndex logicalLightIndex);
        void modify(const LogicalIndex logicalLightIndex, const Light& light);
    };
}

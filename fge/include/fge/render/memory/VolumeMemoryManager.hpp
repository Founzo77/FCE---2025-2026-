#pragma once

#include "DefaultHeapBuffer.hpp"

#include "../indices.hpp"

#include "../../memory/SafePageBasedAllocator.hpp"
#include "../../memory/SlotAllocator.hpp"

#include "../../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Volume;

    class VolumeMemoryManager
    {
    public:
        SafePageBasedAllocator m_volumesPool;
        DefaultHeapBuffer m_scratchBlasBuffers;
        uint64_t m_maxBlasScratchSize = 0;
        vector<uint32_t> m_logicalIndexToPhysical;
        vector<uint32_t> m_physicalIndexToLogical;

    public:
        VolumeMemoryManager() = default;
        ~VolumeMemoryManager();

        void startInitialize(ComPtr<ID3D12Device5> device,
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void moveAllToDevice(ComPtr<ID3D12Device5> device,
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endInitialize(ComPtr<ID3D12Device5> device,
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        void reset();

        uint64_t getVolumePageSize() const;
        uint64_t getNbMaxVolumes() const;
        uint32_t getNbVolumes() const;

        PhysicalIndex logicalToPhysicalIndex(const LogicalIndex logicalIndex) const;
        LogicalIndex physicalToLogicalIndex(const PhysicalIndex physicalIndex) const;

        Volume& get(const LogicalIndex volumeIndex);
        const Volume& get(const LogicalIndex volumeIndex) const;

        SafePageBasedAllocator::OccupiedElementRange<Volume> iterateOverVolumes();
        SafePageBasedAllocator::IndexedOccupiedElementRange<Volume> iterateOverIndexVolumes();
        SafePageBasedAllocator::IndexedOccupiedElementRange<const Volume> 
            iterateOverIndexVolumes() const;

        PhysicalIndex add(Volume&& volume, const LogicalIndex volumeIndex);
        void share(const LogicalIndex volumeIndex);
        bool free(const LogicalIndex volumeIndex);
    };
}
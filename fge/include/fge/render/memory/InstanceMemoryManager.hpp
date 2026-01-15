#pragma once

#include "UploadBuffer.hpp"
#include "DefaultDeviceBuffer.hpp"

#include "../indices.hpp"
#include "../../memory/PageBasedAllocator.hpp"
#include "../../memory/CompactStackAllocator.hpp"
#include "../../memory/IndexRange.hpp"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Instance;
    class MeshMemoryManager;

    class InstanceMemoryManager
    {
    public:
        PageBasedAllocator m_instancesPool;
        vector<uint32_t> m_logicalIndexToPhysical;
        UploadBuffer m_instanceDescUploadBuffer;
        DefaultDeviceBuffer m_tlasBuffer;
        DefaultDeviceBuffer m_scratchTlasBuffer;
        bool m_hastTlasTransformed;
        bool m_hasTlasSizeChanged;

        vector<uint32_t> m_logicalIndexToPhysicalInstanceDesc;
        CompactStackAllocator m_instanceDescPool;
        vector<IndexRange> m_logicalInstancesUpdated;

    public:
        InstanceMemoryManager() = default;
        ~InstanceMemoryManager() = default;

        void startInitialize();
        void moveAllToDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, MeshMemoryManager& meshMemory);
    private:
        void initializeTlas(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, MeshMemoryManager& meshMemory);
    public:
        void endInitialize();

        uint64_t getInstancePageSize() const;
        uint64_t getNbMaxInstances() const;
        uint32_t getNbInstances() const;

        D3D12_GPU_VIRTUAL_ADDRESS getTlasBufferAddress();

        Instance& get(const LogicalIndex instanceIndex);
        const Instance& get(const LogicalIndex instanceIndex) const;

        PageBasedAllocator::OccupiedElementRange<Instance> iterateOverInstances();
        
        void updateDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList, MeshMemoryManager& meshMemory);
    private:
        void updateInstanceDescUploadBuffer(ComPtr<ID3D12Device5> device, 
            MeshMemoryManager& meshMemory);
        void updateResizedTlas(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void updateTransformedTlas(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
    
    public:
        PhysicalIndex add(const Instance& instance, const LogicalIndex instanceIndex);
        void free(const LogicalIndex instanceIndex);

        void modify(const LogicalIndex instanceIndex, const Instance& instance);
    };
}

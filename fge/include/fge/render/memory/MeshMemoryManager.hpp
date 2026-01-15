#pragma once

#include "DefaultHeapBuffer.hpp"

#include "../indices.hpp"

#include "../../memory/SafePageBasedAllocator.hpp"
#include "../../memory/SlotAllocator.hpp"

#include <Windows.h>
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Mesh;

    class MeshMemoryManager
    {
    public:
        SafePageBasedAllocator m_meshesPool;
        SlotAllocator m_subMeshesIndexPool;
        DefaultHeapBuffer m_scratchBlasBuffers;
        uint64_t m_maxBlasScratchSize;
        uint32_t m_nbSubMeshes;
        vector<uint32_t> m_logicalIndexToPhysical;
        vector<uint32_t> m_physicalIndexToLogical;

    public:
        MeshMemoryManager() = default;
        ~MeshMemoryManager() = default;

        void startInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void moveAllToDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        uint64_t getMeshPageSize() const;
        uint64_t getSubMeshDataPageSize() const;
        uint64_t getNbMaxMeshes() const;
        uint64_t getNbMaxSubMeshes() const;
        uint32_t getNbMeshes() const;
        uint32_t getNbSubMeshes() const;

        PhysicalIndex logicalToPhysicalIndex(const LogicalIndex logicalIndex) const;
        LogicalIndex physicalToLogicalIndex(const PhysicalIndex physicalIndex) const;

        Mesh& get(const LogicalIndex meshIndex);
        const Mesh& get(const LogicalIndex meshIndex) const;

        SafePageBasedAllocator::OccupiedElementRange<Mesh> iterateOverMeshes();
        SafePageBasedAllocator::IndexedOccupiedElementRange<Mesh> iterateOverIndexMeshes();
        SafePageBasedAllocator::IndexedOccupiedElementRange<const Mesh> iterateOverIndexMeshes() const;

        PhysicalIndex add(Mesh&& mesh, const LogicalIndex meshIndex);
        void share(const LogicalIndex meshIndex);
        bool free(const LogicalIndex meshIndex);
    };
}

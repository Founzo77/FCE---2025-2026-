#pragma once

#include "UploadBuffer.hpp"
#include "DefaultDeviceBuffer.hpp"

#include "../indices.hpp"

#include "../../memory/SafeCompactStackAllocator.hpp"
#include "../../memory/IndexRange.hpp"

#include "../../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Material;

    class MaterialMemoryManager
    {
    public:
        SafeCompactStackAllocator m_materialsPool;
        vector<uint32_t> m_logicalIndexToPhysical;
        vector<IndexRange> m_physicalMaterialsUpdated;

        ComPtr<ID3D12Heap> m_deviceHeap;
        ComPtr<ID3D12Resource> m_deviceBuffer;
        UploadBuffer m_uploadBuffer;
        UploadBuffer m_indirectionTableUploadBuffer;
        DefaultDeviceBuffer m_indirectionTableDeviceBuffer;

    public:
        MaterialMemoryManager() = default;
        ~MaterialMemoryManager();

        void startInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void moveAllToDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        void reset();

        uint64_t getMaterialPageSize() const;
        uint64_t getNbMaxMaterials() const;
        uint32_t getNbMaterials() const;

        Material& get(const LogicalIndex logicalMaterialIndex);
        const Material& get(const LogicalIndex logicalMaterialIndex) const;

        SafeCompactStackAllocator::OccupiedElementRange<Material> iterateOverMaterials();

        void updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList);

        PhysicalIndex add(const Material& material, const LogicalIndex logicalMaterialIndex);
        void share(const LogicalIndex logicalMaterialIndex);
        bool free(const LogicalIndex logicalMaterialIndex);

        void modify(const LogicalIndex logicalMaterialIndex, const Material& material);
    };
}

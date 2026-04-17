#pragma once

#include "UploadBuffer.hpp"
#include "DefaultDeviceBuffer.hpp"

#include "../../memory/IndexRange.hpp"
#include "../../memory/SafePageBasedAllocator.hpp"

#include "../indices.hpp"

#include "../../PlatformWindows.hpp"
#include <wrl/client.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Texture;

    class TextureMemoryManager
    {
    public:
        SafePageBasedAllocator m_texturesPool;
        vector<uint32_t> m_logicalIndexToPhysical;
        vector<IndexRange> m_physicalMaterialsUpdated;

        UploadBuffer m_indirectionTableUploadBuffer;
        DefaultDeviceBuffer m_indirectionTableDeviceBuffer;

    public:
        TextureMemoryManager() = default;
        ~TextureMemoryManager();

        void startInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void moveAllToDevice(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        void endInitialize(ComPtr<ID3D12Device5> device, 
            ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        
        void reset();
        
        uint64_t getTexturePageSize() const;
        uint64_t getNbMaxTextures() const;

        uint32_t getNbTextures() const;

        Texture& get(const LogicalIndex logicalTextureIndex);
        const Texture& get(const LogicalIndex logicalTextureIndex) const;

        SafePageBasedAllocator::OccupiedElementRange<Texture> iterateOverTextures();
        
        void updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList);
        
        PhysicalIndex add(Texture&& texture, const LogicalIndex logicalTextureIndex);
        void share(const LogicalIndex logicalTextureIndex);
        bool free(const LogicalIndex logicalTextureIndex);
    };
}

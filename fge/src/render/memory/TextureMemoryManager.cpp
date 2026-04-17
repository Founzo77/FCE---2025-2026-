#include <fge/render/memory/TextureMemoryManager.hpp>
#include <fge/render/data/Texture.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

#include <algorithm>

namespace fge
{
    TextureMemoryManager::~TextureMemoryManager()
    {
        reset();
    }

    void TextureMemoryManager::startInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_texturesPool.initialize(getNbMaxTextures(), getTexturePageSize());
        m_logicalIndexToPhysical.resize(getNbMaxTextures(), UINT32_MAX);
    }

    void TextureMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        for(Texture& texture : iterateOverTextures())
        {
            texture.m_deviceBuffer.initialize(device, texture.m_metaData);
            d12SetDebugName(texture.m_deviceBuffer.getTexture(),
                L"Texture Buffer");

            // TO_DO Integrer par rapport aux mimpmap et autres si il y a plusieurs
            // images
            const Image* image = texture.m_image.GetImage(0, 0, 0);
            
            /*
            TO_DO Recommande compatible avec Mipmaps / Texture arrays / Texture 3D,2D
            uint32_t subresourceCount =
                texture.m_metaData.mipLevels * texture.m_metaData.arraySize;

            uint64_t uploadBufferSize = GetRequiredIntermediateSize(
                texture.m_deviceBuffer.getTexture().Get(),
                0,
                subresourceCount);
            */

            // TO_DO est ce que uploadBufferSize commum a toutes les images ?
            // Si oui on peut peut etre avoir un seul buffer
            uint64_t uploadBufferSize = GetRequiredIntermediateSize(
                texture.m_deviceBuffer.getTexture().Get(), 0, 1);

            texture.m_uploadBuffer.initialize(device, uploadBufferSize);
            d12SetDebugName(texture.m_uploadBuffer.m_buffer, L"Texture Upload Buffer");

            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData = image->pixels;
            textureData.RowPitch = image->rowPitch;
            textureData.SlicePitch = image->slicePitch;

            UpdateSubresources(directCommandList.Get(), texture.m_deviceBuffer.getTexture().Get(), 
                texture.m_uploadBuffer.getBuffer().Get(), 0, 0, 1, &textureData);

            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                texture.m_deviceBuffer.getTexture().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | 
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            directCommandList->ResourceBarrier(1, &barrier);
        }
    }

    void TextureMemoryManager::endInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {

    }

    void TextureMemoryManager::reset()
    {
        if(m_texturesPool.getNbElements() > 0)
        {
            for(Texture& texture : iterateOverTextures())
            {
                texture.reset();
            }
        }
        m_texturesPool.reset();
     
        m_logicalIndexToPhysical.clear();
        m_logicalIndexToPhysical.shrink_to_fit();
        m_physicalMaterialsUpdated.clear();
        m_physicalMaterialsUpdated.shrink_to_fit();
        m_indirectionTableUploadBuffer.reset();
        m_indirectionTableDeviceBuffer.reset();
    }

    uint64_t TextureMemoryManager::getTexturePageSize() const
    {
        return sizeof(Texture);
    }

    uint64_t TextureMemoryManager::getNbMaxTextures() const
    {
        return 1000;
    }

    uint32_t TextureMemoryManager::getNbTextures() const
    {
        return m_texturesPool.getNbElements();
    }

    Texture& TextureMemoryManager::get(const LogicalIndex logicalTextureIndex)
    {
        return m_texturesPool.get<Texture>(m_logicalIndexToPhysical.at(logicalTextureIndex.m_index));
    }

    const Texture& TextureMemoryManager::get(const LogicalIndex logicalTextureIndex) const
    {
        return m_texturesPool.get<Texture>(m_logicalIndexToPhysical.at(logicalTextureIndex.m_index));
    }

    SafePageBasedAllocator::OccupiedElementRange<Texture> 
        TextureMemoryManager::iterateOverTextures()
    {
        return m_texturesPool.occupiedElements<Texture>();
    }

    void TextureMemoryManager::updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        // TO_DO : utilise m_physicalMaterialsUpdated avec une structure qui gere directement 
        // le stockage des IndexRange merge
        /*
        const uint64_t strideBytes = m_texturesPool.getStrideSize();
        const uint64_t occupiedBytes = m_texturesPool.getTotalSizeOccupy();
        uint8_t* cpuBase = m_texturesPool.getBuffer();

        if (m_physicalMaterialsUpdated.empty())
            return;

        std::sort(m_physicalMaterialsUpdated.begin(), m_physicalMaterialsUpdated.end(),
            [](const IndexRange& a, const IndexRange& b){ return a.m_index < b.m_index; });

        std::vector<IndexRange> merged;

        merged.reserve(m_physicalMaterialsUpdated.size());

        for (const auto& r : m_physicalMaterialsUpdated) 
        {
            if (r.m_count == 0) continue;

            if (merged.empty()) { merged.push_back(r); continue; }

            auto& back = merged.back();

            const uint32_t backEnd = back.m_index + back.m_count;

            if (r.m_index <= backEnd) 
            {
                const uint32_t newEnd = max(backEnd, r.m_index + r.m_count);
                back.m_count = newEnd - back.m_index;
            }
            else 
            {
                merged.push_back(r);
            }
        }

        void* mappedUploadBuffer = m_uploadBuffer.startUpload();
        uint8_t* upBaseUploadBuffer = reinterpret_cast<uint8_t*>(mappedUploadBuffer);

        void* mappedIndirectionTableUploadBuffer = m_indirectionTableUploadBuffer.startUpload();
        uint8_t* upBaseIndirectionTableUploadBuffer = 
            reinterpret_cast<uint8_t*>(mappedIndirectionTableUploadBuffer);

        for (const auto& indexRange : merged) 
        {
            const uint64_t byteOffset = uint64_t(indexRange.m_index) * strideBytes;
            const uint64_t byteSize = uint64_t(indexRange.m_count) * strideBytes;

            std::memcpy(upBaseUploadBuffer + byteOffset, cpuBase + byteOffset, byteSize);
            
            const uint64_t byteIndirectionTableOffset = 
                uint64_t(indexRange.m_index) * sizeof(uint32_t);
            const uint64_t byteIndirectionTableSize = 
                uint64_t(indexRange.m_count) * sizeof(uint32_t);

            std::memcpy(upBaseIndirectionTableUploadBuffer + byteIndirectionTableOffset, 
                cpuBase + byteIndirectionTableOffset, byteIndirectionTableSize);
        }

        m_uploadBuffer.endUpload();
        m_indirectionTableUploadBuffer.endUpload();
        
        CD3DX12_RESOURCE_BARRIER barrier[2] = { 
            CD3DX12_RESOURCE_BARRIER::Transition(m_deviceBuffer.Get(), 
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(m_indirectionTableDeviceBuffer.getBuffer().Get(), 
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
        };
        directCommandList->ResourceBarrier(2, barrier);

        for (const auto& indexRange : merged) 
        {
            const uint64_t byteOffset = uint64_t(indexRange.m_index) * strideBytes;
            const uint64_t byteSize = uint64_t(indexRange.m_count) * strideBytes;

            directCommandList->CopyBufferRegion(
                m_deviceBuffer.Get(), byteOffset,
                m_uploadBuffer.getBuffer().Get(), byteOffset,
                byteSize);

            const uint64_t byteIndirectionTableOffset = 
                uint64_t(indexRange.m_index) * sizeof(uint32_t);
            const uint64_t byteIndirectionTableSize = 
                uint64_t(indexRange.m_count) * sizeof(uint32_t);

            directCommandList->CopyBufferRegion(
                m_indirectionTableDeviceBuffer.getBuffer().Get(), byteIndirectionTableOffset,
                m_indirectionTableUploadBuffer.getBuffer().Get(), byteIndirectionTableOffset,
                byteIndirectionTableSize);
        }

        barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_deviceBuffer.Get(), 
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
            m_indirectionTableDeviceBuffer.getBuffer().Get(), 
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        directCommandList->ResourceBarrier(2, barrier);

        m_physicalMaterialsUpdated.clear();
        */
    }

    PhysicalIndex TextureMemoryManager::add(Texture&& texture, const LogicalIndex logicalTextureIndex)
    {
        uint32_t physicalTextureIndex = m_texturesPool.alloc();
        m_texturesPool.get<Texture>(physicalTextureIndex) = std::move(texture);
        m_logicalIndexToPhysical[logicalTextureIndex.m_index] = physicalTextureIndex;

        return { physicalTextureIndex };
    }

    void TextureMemoryManager::share(const LogicalIndex logicalTextureIndex)
    {
        m_texturesPool.share(logicalTextureIndex.m_index);
    }

    bool TextureMemoryManager::free(const LogicalIndex logicalTextureIndex)
    {
        get(logicalTextureIndex).reset();
        bool has_freed = m_texturesPool.free(logicalTextureIndex.m_index);

        if(has_freed)
        {
            m_logicalIndexToPhysical[logicalTextureIndex.m_index] = UINT32_MAX;
        }

        return has_freed;
    }
}


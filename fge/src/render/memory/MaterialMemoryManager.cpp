#include <fge/render/memory/MaterialMemoryManager.hpp>
#include <fge/render/data/Material.hpp>

#include <fge/utility.hpp>

#include <fge/io/GlobalLogger.hpp>

#include <d3dx12.h>

#include <algorithm>

namespace fge
{
    MaterialMemoryManager::~MaterialMemoryManager()
    {
        reset();
    }
    
    void MaterialMemoryManager::startInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_materialsPool.initialize(getNbMaxMaterials(), getMaterialPageSize());

        D3D12_HEAP_DESC desc = {};
        desc.SizeInBytes = alignData(m_materialsPool.getBufferTotalSize(), 
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Flags = D3D12_HEAP_FLAG_NONE;

        throwIfFailed(device->CreateHeap(&desc, IID_PPV_ARGS(&m_deviceHeap)),
            "Failed to create ID3D12Heap");
        d12SetDebugName(m_deviceHeap, L"Material Heap");

        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
            m_materialsPool.getBufferTotalSize());

        throwIfFailed(device->CreatePlacedResource(m_deviceHeap.Get(), 0,
            &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_deviceBuffer)),
            "Failed to create placed material buffer");
        d12SetDebugName(m_deviceBuffer, L"Material Heap Buffer");

        m_uploadBuffer.initialize(device, m_materialsPool.getBufferTotalSize());
        d12SetDebugName(m_uploadBuffer.m_buffer, L"Material Heap Upload Buffer");

        m_indirectionTableUploadBuffer.initialize(device, 
            m_materialsPool.getNbMaxElements() * sizeof(uint32_t));
        d12SetDebugName(m_indirectionTableUploadBuffer.m_buffer, 
            L"Material Indirection Table Upload Buffer");
        m_indirectionTableDeviceBuffer.initialize(device, 
            m_materialsPool.getNbMaxElements() * sizeof(uint32_t), D3D12_RESOURCE_FLAG_NONE,
            D3D12_RESOURCE_STATE_COPY_DEST);
        d12SetDebugName(m_indirectionTableDeviceBuffer.m_buffer, 
            L"Material Indirection Table Buffer");

        m_logicalIndexToPhysical.resize(m_materialsPool.getNbMaxElements(), UINT32_MAX);
    }

    void MaterialMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        throwIfFailed(!m_deviceBuffer == false, "Device Buffer is not initialized");
        throwIfFailed(m_materialsPool.getNbElements() > 0, "Materials are empty");

        m_uploadBuffer.upload(m_materialsPool.getBuffer(), m_materialsPool.getTotalSizeOccupy());
        directCommandList->CopyBufferRegion(m_deviceBuffer.Get(), 0, 
            m_uploadBuffer.getBuffer().Get(), 0, m_materialsPool.getTotalSizeOccupy());

        m_indirectionTableUploadBuffer.upload(m_logicalIndexToPhysical.data(),
            m_materialsPool.getNbMaxElements() * sizeof(uint32_t));
        directCommandList->CopyBufferRegion(m_indirectionTableDeviceBuffer.getBuffer().Get(), 0, 
            m_indirectionTableUploadBuffer.getBuffer().Get(), 0, 
            m_materialsPool.getNbMaxElements() * sizeof(uint32_t));

        m_physicalMaterialsUpdated.clear();
    }

    void MaterialMemoryManager::endInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        throwIfFailed(!m_deviceBuffer == false, "Device Buffer is not initialized");

        CD3DX12_RESOURCE_BARRIER barrier[2] = { 
            CD3DX12_RESOURCE_BARRIER::Transition(m_deviceBuffer.Get(), 
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(m_indirectionTableDeviceBuffer.getBuffer().Get(), 
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
        };
        directCommandList->ResourceBarrier(2, barrier);
    }

    void MaterialMemoryManager::reset()
    {
        m_materialsPool.reset();
        
        m_logicalIndexToPhysical.clear();
        m_logicalIndexToPhysical.shrink_to_fit();
        m_physicalMaterialsUpdated.clear();
        m_physicalMaterialsUpdated.shrink_to_fit();
        
        m_deviceHeap.Reset();
        
        m_deviceBuffer.Reset();
        m_uploadBuffer.reset();

        m_indirectionTableUploadBuffer.reset();
        m_indirectionTableDeviceBuffer.reset();
    }

    uint64_t MaterialMemoryManager::getMaterialPageSize() const
    {
        return alignData(sizeof(Material), 16);
    }

    uint64_t MaterialMemoryManager::getNbMaxMaterials() const
    {
        return 1000;
    }

    uint32_t MaterialMemoryManager::getNbMaterials() const
    {
        return m_materialsPool.getNbElements();
    }

    Material& MaterialMemoryManager::get(const LogicalIndex logicalMaterialIndex)
    {
        return m_materialsPool.get<Material>(
            m_logicalIndexToPhysical.at(logicalMaterialIndex.m_index));
    }

    const Material& MaterialMemoryManager::get(const LogicalIndex logicalMaterialIndex) const
    {
        return m_materialsPool.get<Material>(
            m_logicalIndexToPhysical.at(logicalMaterialIndex.m_index));
    }

    SafeCompactStackAllocator::OccupiedElementRange<Material> 
        MaterialMemoryManager::iterateOverMaterials()
    {
        return m_materialsPool.occupiedElements<Material>();
    }

    void MaterialMemoryManager::updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        // TO_DO : utilise m_physicalMaterialsUpdated avec une structure qui gere directement 
        // le stockage des IndexRange merge

        const uint64_t strideBytes = m_materialsPool.getStrideSize();
        const uint64_t occupiedBytes = m_materialsPool.getTotalSizeOccupy();

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
                const uint32_t newEnd = std::max(backEnd, r.m_index + r.m_count);
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

        uint8_t* materialBuffer = m_materialsPool.getBuffer();

        for (const auto& indexRange : merged) 
        {
            const uint64_t byteOffset = uint64_t(indexRange.m_index) * strideBytes;
            const uint64_t byteSize = uint64_t(indexRange.m_count) * strideBytes;

            std::memcpy(upBaseUploadBuffer + byteOffset, materialBuffer + byteOffset, byteSize);
            
            const uint64_t byteIndirectionTableOffset = 
                uint64_t(indexRange.m_index) * sizeof(uint32_t);

            std::memcpy(upBaseIndirectionTableUploadBuffer + byteIndirectionTableOffset, 
                &m_logicalIndexToPhysical[indexRange.m_index], 
                indexRange.m_count * sizeof(uint32_t));
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
    }

    PhysicalIndex MaterialMemoryManager::add(
        const Material& material, const LogicalIndex logicalMaterialIndex)
    {
        uint32_t physicalMaterialIndex = m_materialsPool.alloc();
        m_materialsPool.get<Material>(physicalMaterialIndex) = material;
        m_logicalIndexToPhysical[logicalMaterialIndex.m_index] = physicalMaterialIndex;

        return { physicalMaterialIndex };
    }

    void MaterialMemoryManager::share(const LogicalIndex logicalMaterialIndex)
    {
        m_materialsPool.share(m_logicalIndexToPhysical[logicalMaterialIndex.m_index]);
    }

    bool MaterialMemoryManager::free(const LogicalIndex logicalMaterialIndex)
    {
        bool has_freed = m_materialsPool.free(m_logicalIndexToPhysical[logicalMaterialIndex.m_index]);

        if(has_freed)
        {
            m_logicalIndexToPhysical[logicalMaterialIndex.m_index] = UINT32_MAX;
        }

        return has_freed;
    }

    void MaterialMemoryManager::modify(
        const LogicalIndex logicalMaterialIndex, const Material& material)
    {
        m_materialsPool.get<Material>(
            m_logicalIndexToPhysical[logicalMaterialIndex.m_index]) = material;
        m_physicalMaterialsUpdated.push_back(
            { m_logicalIndexToPhysical[logicalMaterialIndex.m_index], 1 });
    }
}


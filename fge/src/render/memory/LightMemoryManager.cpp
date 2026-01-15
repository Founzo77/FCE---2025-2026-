#include <fge/render/memory/LightMemoryManager.hpp>
#include <fge/render/data/Light.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

#include <algorithm>

namespace fge
{
    void LightMemoryManager::startInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_lightsPool.initialize(getNbMaxLights(), getLightPageSize());

        D3D12_HEAP_DESC desc = {};
        desc.SizeInBytes = alignData(m_lightsPool.getBufferTotalSize(), 
            D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
        desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        desc.Flags = D3D12_HEAP_FLAG_NONE;

        throwIfFailed(device->CreateHeap(&desc, IID_PPV_ARGS(&m_deviceHeap)),
            "Failed to create ID3D12Heap");

        D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
            m_lightsPool.getBufferTotalSize());

        throwIfFailed(device->CreatePlacedResource(m_deviceHeap.Get(), 0,
            &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_deviceBuffer)),
            "Failed to create placed light buffer");

        m_uploadBuffer.initialize(device, m_lightsPool.getBufferTotalSize());
    }

    void LightMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        throwIfFailed(!m_deviceBuffer == false, "Device Buffer is not initialized");
        throwIfFailed(m_lightsPool.getNbElements() > 0, "Lights are empty");

        m_uploadBuffer.upload(m_lightsPool.getBuffer(), m_lightsPool.getTotalSizeOccupy());

        directCommandList->CopyBufferRegion(m_deviceBuffer.Get(), 0, 
            m_uploadBuffer.getBuffer().Get(), 0, m_lightsPool.getTotalSizeOccupy());

        m_physicalLightsUpdated.clear();
        m_hasLightsResized = true;
    }

    void LightMemoryManager::endInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        throwIfFailed(!m_deviceBuffer == false, "Device Buffer is not initialized");

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_deviceBuffer.Get(), 
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        directCommandList->ResourceBarrier(1, &barrier);
    }

    uint64_t LightMemoryManager::getLightPageSize() const
    {
        return alignData(sizeof(Light), 16);
    }

    uint64_t LightMemoryManager::getNbMaxLights() const
    {
        return 1000;
    }

    uint32_t LightMemoryManager::getNbLights() const
    {
        return m_lightsPool.getNbElements();
    }

    Light& LightMemoryManager::get(const LogicalIndex logicalLightIndex)
    {
        return m_lightsPool.get<Light>(m_logicalIndexToPhysical.at(logicalLightIndex.m_index));
    }

    const Light& LightMemoryManager::get(const LogicalIndex logicalLightIndex) const
    {
        return m_lightsPool.get<Light>(m_logicalIndexToPhysical.at(logicalLightIndex.m_index));
    }

    CompactStackAllocator::OccupiedElementRange<Light> LightMemoryManager::iterateOverLights()
    {
        return m_lightsPool.occupiedElements<Light>();
    }

    void LightMemoryManager::updateDevice(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        // TO_DO : utilise m_physicalLightsUpdated avec une structure qui gere directement 
        // le stockage des IndexRange merge

        const uint64_t strideBytes = m_lightsPool.getStrideSize();
        const uint64_t occupiedBytes = m_lightsPool.getTotalSizeOccupy();
        uint8_t* cpuBase = m_lightsPool.getBuffer();

        if (m_physicalLightsUpdated.empty())
            return;

        std::sort(m_physicalLightsUpdated.begin(), m_physicalLightsUpdated.end(),
            [](const IndexRange& a, const IndexRange& b){ return a.m_index < b.m_index; });

        std::vector<IndexRange> merged;

        merged.reserve(m_physicalLightsUpdated.size());

        for (const auto& r : m_physicalLightsUpdated) 
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

        void* mapped = m_uploadBuffer.startUpload();
        uint8_t* upBase = reinterpret_cast<uint8_t*>(mapped);

        for (const auto& indexRange : merged) 
        {
            const uint64_t byteOffset = uint64_t(indexRange.m_index) * strideBytes;
            const uint64_t byteSize = uint64_t(indexRange.m_count) * strideBytes;

            std::memcpy(upBase + byteOffset, cpuBase + byteOffset, static_cast<size_t>(byteSize));
        }

        m_uploadBuffer.endUpload();
        
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_deviceBuffer.Get(), 
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
        directCommandList->ResourceBarrier(1, &barrier);

        for (const auto& indexRange : merged) 
        {
            const uint64_t byteOffset = uint64_t(indexRange.m_index) * strideBytes;
            const uint64_t byteSize = uint64_t(indexRange.m_count) * strideBytes;

            directCommandList->CopyBufferRegion(
                m_deviceBuffer.Get(), byteOffset,
                m_uploadBuffer.getBuffer().Get(), byteOffset,
                byteSize);
        }

        barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_deviceBuffer.Get(), 
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        directCommandList->ResourceBarrier(1, &barrier);

        m_physicalLightsUpdated.clear();
    }

    PhysicalIndex LightMemoryManager::add(const Light& light, const LogicalIndex logicalLightIndex)
    {
        uint32_t physicalLightIndex = m_lightsPool.alloc(m_physicalLightsUpdated);
        m_lightsPool.get<Light>(physicalLightIndex) = light;
        m_logicalIndexToPhysical.insert({ logicalLightIndex.m_index, physicalLightIndex});

        m_hasLightsResized = true;

        return { physicalLightIndex };
    }

    void LightMemoryManager::free(const LogicalIndex logicalLightIndex)
    {
        m_lightsPool.free(m_logicalIndexToPhysical[logicalLightIndex.m_index], m_physicalLightsUpdated);
        m_logicalIndexToPhysical.erase(logicalLightIndex.m_index);

        m_hasLightsResized = true;
    }

    void LightMemoryManager::modify(const LogicalIndex logicalLightIndex, const Light& light)
    {
        m_lightsPool.get<Light>(m_logicalIndexToPhysical[logicalLightIndex.m_index]) = light;
        m_physicalLightsUpdated.push_back(
            { m_logicalIndexToPhysical[logicalLightIndex.m_index], 1 });
    }
}


#include <fge/render/memory/VolumeMemoryManager.hpp>
#include <fge/render/data/Volume.hpp>

#include <fge/utility.hpp>
#include <d3dx12.h>

namespace fge
{
    VolumeMemoryManager::~VolumeMemoryManager()
    {
        reset();
    }

    void VolumeMemoryManager::startInitialize(ComPtr<ID3D12Device5> device,
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_volumesPool.initialize(getNbMaxVolumes(), getVolumePageSize());

        m_maxBlasScratchSize = 0;

        m_logicalIndexToPhysical.resize(getNbMaxVolumes(), UINT32_MAX);
        m_physicalIndexToLogical.resize(getNbMaxVolumes(), UINT32_MAX);
    }

    void VolumeMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device,
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_maxBlasScratchSize = 0;

        for (Volume& volume : iterateOverVolumes())
        {
            // Crée AABB buffer + upload
            D3D12_RAYTRACING_AABB aabb = {};
            aabb.MinX = volume.m_volumeData.m_mins[0]; aabb.MinY = volume.m_volumeData.m_mins[1]; 
            aabb.MinZ = volume.m_volumeData.m_mins[2];
            aabb.MaxX = volume.m_volumeData.m_maxs[0]; aabb.MaxY = volume.m_volumeData.m_maxs[1]; 
            aabb.MaxZ = volume.m_volumeData.m_maxs[2];

            const uint64_t aabbBufferSize = sizeof(D3D12_RAYTRACING_AABB);

            volume.m_aabbBuffer.initialize(device, aabbBufferSize, D3D12_RESOURCE_FLAG_NONE);
            d12SetDebugName(volume.m_aabbBuffer.m_buffer, L"Volume Buffer");

            CD3DX12_RESOURCE_BARRIER toCopy = CD3DX12_RESOURCE_BARRIER::Transition(
                volume.m_aabbBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
            directCommandList->ResourceBarrier(1, &toCopy);
            
            volume.m_aabbUploadBuffer.initialize(device, aabbBufferSize);
            d12SetDebugName(volume.m_aabbUploadBuffer.m_buffer, L"Volume Upload Buffer");
            volume.m_aabbUploadBuffer.upload(&aabb, aabbBufferSize);

            directCommandList->CopyBufferRegion(
                volume.m_aabbBuffer.getBuffer().Get(), 0,
                volume.m_aabbUploadBuffer.getBuffer().Get(), 0,
                aabbBufferSize);

            CD3DX12_RESOURCE_BARRIER toSrv = CD3DX12_RESOURCE_BARRIER::Transition(
                volume.m_aabbBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            directCommandList->ResourceBarrier(1, &toSrv);

            // 

            VolumeData volumeData = volume.m_volumeData;
            const uint64_t volumeDataSize = sizeof(VolumeData);

            volume.m_volumeDataBuffer.initialize(device, volumeDataSize, D3D12_RESOURCE_FLAG_NONE);
            d12SetDebugName(volume.m_volumeDataBuffer.m_buffer, L"Volume Data Buffer");

            toCopy = CD3DX12_RESOURCE_BARRIER::Transition(
                volume.m_volumeDataBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COMMON,
                D3D12_RESOURCE_STATE_COPY_DEST);
            directCommandList->ResourceBarrier(1, &toCopy);

            volume.m_volumeDataUploadBuffer.initialize(device, volumeDataSize);
            d12SetDebugName(volume.m_volumeDataUploadBuffer.m_buffer, L"Volume Data Upload Buffer");
            volume.m_volumeDataUploadBuffer.upload(&volumeData, volumeDataSize);

            directCommandList->CopyBufferRegion(
                volume.m_volumeDataBuffer.getBuffer().Get(), 0,
                volume.m_volumeDataUploadBuffer.getBuffer().Get(), 0,
                volumeDataSize);

            toSrv = CD3DX12_RESOURCE_BARRIER::Transition(
                volume.m_volumeDataBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            directCommandList->ResourceBarrier(1, &toSrv);

            // GeometryDesc : AABBs procédurales
            D3D12_RAYTRACING_GEOMETRY_DESC geom = {};
            geom.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
            geom.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

            geom.AABBs.AABBCount = 1;
            geom.AABBs.AABBs.StartAddress = volume.m_aabbBuffer.getBuffer()->GetGPUVirtualAddress();
            geom.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);

            volume.m_geometryDesc = geom;

            // BLAS inputs
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInput = {};
            blasInput.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
            blasInput.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            blasInput.NumDescs = 1;
            blasInput.pGeometryDescs = &volume.m_geometryDesc;
            blasInput.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

            // Prebuild + alloc BLAS
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
            device->GetRaytracingAccelerationStructurePrebuildInfo(&blasInput, &prebuildInfo);

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildBlasDesc = {};
            buildBlasDesc.Inputs = blasInput;

            m_maxBlasScratchSize = std::max(m_maxBlasScratchSize, prebuildInfo.ScratchDataSizeInBytes);

            uint64_t blasSize = alignData(
                prebuildInfo.ResultDataMaxSizeInBytes,
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

            volume.m_blasBuffer.initialize(device, blasSize,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
            d12SetDebugName(volume.m_blasBuffer.m_buffer, L"Volume Blas Buffer");

            volume.m_blasDescription = buildBlasDesc;
        }

        // Scratch BLAS partagé
        m_scratchBlasBuffers.initialize(device, m_maxBlasScratchSize,
            D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
        m_scratchBlasBuffers.allocate(device, m_maxBlasScratchSize,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        d12SetDebugName(m_scratchBlasBuffers.m_heap, L"Scratch Volume Blas Heap");
        for(ComPtr<ID3D12Resource> blasBuffer : m_scratchBlasBuffers.m_buffers)
            d12SetDebugName(blasBuffer, L"Scratch Volume Blas Heap Buffer");

        // Build BLAS
        for (Volume& volume : iterateOverVolumes())
        {
            volume.m_blasDescription.ScratchAccelerationStructureData =
                m_scratchBlasBuffers.getBuffer(0)->GetGPUVirtualAddress();
            volume.m_blasDescription.DestAccelerationStructureData =
                volume.m_blasBuffer.getBuffer()->GetGPUVirtualAddress();

            directCommandList->BuildRaytracingAccelerationStructure(
                &volume.m_blasDescription, 0, nullptr);

            CD3DX12_RESOURCE_BARRIER uavBarrier =
                CD3DX12_RESOURCE_BARRIER::UAV(volume.m_blasBuffer.getBuffer().Get());
            directCommandList->ResourceBarrier(1, &uavBarrier);
        }
    }

    void VolumeMemoryManager::endInitialize(ComPtr<ID3D12Device5> device,
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {

    }

    void VolumeMemoryManager::reset()
    {
        if(m_volumesPool.getNbElements() > 0)
        {
            for(Volume& volume : iterateOverVolumes())
            {
                volume.reset();
            }
        }

        m_volumesPool.reset();

        m_scratchBlasBuffers.reset();
        m_maxBlasScratchSize = 0;
        m_logicalIndexToPhysical.clear();
        m_logicalIndexToPhysical.shrink_to_fit();
    }

    uint64_t VolumeMemoryManager::getVolumePageSize() const
    {
        return sizeof(Volume);
    }

    uint64_t VolumeMemoryManager::getNbMaxVolumes() const
    {
        return 1000; // à ajuster
    }

    uint32_t VolumeMemoryManager::getNbVolumes() const
    {
        return m_volumesPool.getNbElements();
    }

    PhysicalIndex VolumeMemoryManager::logicalToPhysicalIndex(const LogicalIndex logicalIndex) const
    {
        return { m_logicalIndexToPhysical[logicalIndex.m_index] };
    }

    LogicalIndex VolumeMemoryManager::physicalToLogicalIndex(const PhysicalIndex physicalIndex) const
    {
        return { m_physicalIndexToLogical[physicalIndex.m_index] };
    }

    Volume& VolumeMemoryManager::get(const LogicalIndex volumeIndex)
    {
        return m_volumesPool.get<Volume>(m_logicalIndexToPhysical[volumeIndex.m_index]);
    }

    const Volume& VolumeMemoryManager::get(const LogicalIndex volumeIndex) const
    {
        return m_volumesPool.get<Volume>(m_logicalIndexToPhysical[volumeIndex.m_index]);
    }

    SafePageBasedAllocator::OccupiedElementRange<Volume> VolumeMemoryManager::iterateOverVolumes()
    {
        return m_volumesPool.occupiedElements<Volume>();
    }

    SafePageBasedAllocator::IndexedOccupiedElementRange<Volume>
        VolumeMemoryManager::iterateOverIndexVolumes()
    {
        return m_volumesPool.indexedOccupiedElements<Volume>();
    }

    SafePageBasedAllocator::IndexedOccupiedElementRange<const Volume>
        VolumeMemoryManager::iterateOverIndexVolumes() const
    {
        return m_volumesPool.indexedOccupiedElements<const Volume>();
    }

    PhysicalIndex VolumeMemoryManager::add(Volume&& volume, const LogicalIndex volumeIndex)
    {
        uint32_t physicalIndex = m_volumesPool.alloc();
        m_logicalIndexToPhysical[volumeIndex.m_index] = physicalIndex;
        m_physicalIndexToLogical[physicalIndex] = volumeIndex.m_index;

        Volume& currentVolume = m_volumesPool.get<Volume>(physicalIndex);
        currentVolume = std::move(volume);
        // TO_DO : S'assurer de ca = physicalIndex
        currentVolume.m_sbtBaseIndex = physicalIndex;

        return { physicalIndex };
    }

    void VolumeMemoryManager::share(const LogicalIndex volumeIndex)
    {
        m_volumesPool.share(m_logicalIndexToPhysical[volumeIndex.m_index]);
    }

    bool VolumeMemoryManager::free(const LogicalIndex volumeIndex)
    {
        get(volumeIndex).reset();
        bool hasFreed = m_volumesPool.free(m_logicalIndexToPhysical[volumeIndex.m_index]);

        if (hasFreed)
        {
            m_physicalIndexToLogical[m_logicalIndexToPhysical[volumeIndex.m_index]] = UINT32_MAX;
            m_logicalIndexToPhysical[volumeIndex.m_index] = UINT32_MAX;

            // TO_DO Voir pour optionnel : libérer aussi l’entrée SBT si tu gardes la valeur
            // m_volumeSbtIndexPool.free(current.m_sbtBaseIndex);
        }

        return hasFreed;
    }
}
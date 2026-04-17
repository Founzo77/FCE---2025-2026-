#include <fge/render/memory/InstanceMemoryManager.hpp>

#include <fge/render/memory/MeshMemoryManager.hpp>
#include <fge/render/memory/VolumeMemoryManager.hpp>

#include <fge/render/data/Instance.hpp>
#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/Volume.hpp>

#include <fge/io/GlobalLogger.hpp>

#include <d3dx12.h>

namespace fge
{
    InstanceMemoryManager::~InstanceMemoryManager()
    {
        reset();
    }

    void InstanceMemoryManager::startInitialize()
    {
        m_instancesPool.initialize(getNbMaxInstances(), getInstancePageSize());
        m_logicalIndexToPhysical.resize(getNbMaxInstances(), UINT32_MAX);
        // TO_DO
    }

    void InstanceMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList, MeshMemoryManager& meshMemory,
        VolumeMemoryManager& volumeMemoryManager)
    {
        uint64_t instancesDescSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * getNbInstances();
        m_instanceDescUploadBuffer.initialize(device, instancesDescSize * 5); // TO_DO : Changer
        d12SetDebugName(m_instanceDescUploadBuffer.m_buffer, L"Instance Desc Upload Buffer");

        updateInstanceDescUploadBuffer(device, meshMemory, volumeMemoryManager);
        initializeTlas(device, directCommandList);

        m_hastTlasTransformed = false;
        m_hasTlasSizeChanged = false;
    }

    void InstanceMemoryManager::initializeTlas(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInput = {};
        tlasInput.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        tlasInput.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
        tlasInput.NumDescs = static_cast<UINT>(getNbInstances());
        tlasInput.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        tlasInput.InstanceDescs = m_instanceDescUploadBuffer.getBuffer()->GetGPUVirtualAddress();

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuildInfo = {};
        device->GetRaytracingAccelerationStructurePrebuildInfo(&tlasInput, &tlasPrebuildInfo);

        uint64_t scratchSize = alignData(tlasPrebuildInfo.ScratchDataSizeInBytes,
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        uint64_t tlasSize = alignData(tlasPrebuildInfo.ResultDataMaxSizeInBytes,
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        
        // TO_DO Changer * 5
        m_scratchTlasBuffer.initialize(device, scratchSize * 5,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        m_tlasBuffer.initialize(device, tlasSize * 5, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
        d12SetDebugName(m_tlasBuffer.m_buffer, L"Tlas Buffer");
        d12SetDebugName(m_scratchTlasBuffer.m_buffer, L"Scratch Tlas Buffer");

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_scratchTlasBuffer.getBuffer().Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        directCommandList->ResourceBarrier(1, &barrier);

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlasDesc = {};
        tlasDesc.Inputs = tlasInput;
        tlasDesc.ScratchAccelerationStructureData = 
            m_scratchTlasBuffer.getBuffer()->GetGPUVirtualAddress();
        tlasDesc.DestAccelerationStructureData =
            m_tlasBuffer.getBuffer()->GetGPUVirtualAddress();

        directCommandList->BuildRaytracingAccelerationStructure(&tlasDesc, 0, nullptr);

        CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(
            m_tlasBuffer.getBuffer().Get());
        directCommandList->ResourceBarrier(1, &uavBarrier);
    }

    void InstanceMemoryManager::endInitialize()
    {
        
    }

    void InstanceMemoryManager::reset()
    {
        m_instancesPool.reset();
        
        m_logicalIndexToPhysical.clear();
        m_logicalIndexToPhysical.shrink_to_fit();

        m_instanceDescUploadBuffer.reset();
        m_tlasBuffer.reset();
        m_scratchTlasBuffer.reset();

        m_hastTlasTransformed = false;
        m_hasTlasSizeChanged = false;

        m_logicalIndexToPhysicalInstanceDesc.clear();
        m_logicalIndexToPhysicalInstanceDesc.shrink_to_fit();
        m_instanceDescPool.reset();
        m_logicalInstancesUpdated.clear();
        m_logicalInstancesUpdated.shrink_to_fit();
    }

    uint64_t InstanceMemoryManager::getInstancePageSize() const
    {
        return sizeof(Instance);
    }

    uint64_t InstanceMemoryManager::getNbMaxInstances() const
    {
        return 1000;
    }

    uint32_t InstanceMemoryManager::getNbInstances() const
    {
        return m_instancesPool.getNbElements();
    }

    D3D12_GPU_VIRTUAL_ADDRESS InstanceMemoryManager::getTlasBufferAddress()
    {
        return m_tlasBuffer.getBuffer()->GetGPUVirtualAddress();
    }

    Instance& InstanceMemoryManager::get(const LogicalIndex instanceIndex)
    {
        return m_instancesPool.get<Instance>(m_logicalIndexToPhysical[instanceIndex.m_index]);
    }

    const Instance& InstanceMemoryManager::get(const LogicalIndex instanceIndex) const
    {
        return m_instancesPool.get<Instance>(m_logicalIndexToPhysical[instanceIndex.m_index]);
    }

    PageBasedAllocator::OccupiedElementRange<Instance> 
        InstanceMemoryManager::iterateOverInstances()
    {
        return m_instancesPool.occupiedElements<Instance>();
    }

    void InstanceMemoryManager::updateDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList, MeshMemoryManager& meshMemory,
        VolumeMemoryManager& volumeMemoryManager)
    {
        if(m_hasTlasSizeChanged)
        {
            updateInstanceDescUploadBuffer(device, meshMemory, volumeMemoryManager);
            updateResizedTlas(device, directCommandList);
            m_hasTlasSizeChanged = false;
            m_hastTlasTransformed = false;
        }
        else if(m_hastTlasTransformed)
        {
            updateInstanceDescUploadBuffer(device, meshMemory, volumeMemoryManager);
            updateTransformedTlas(directCommandList);
            m_hastTlasTransformed = false;
        }
    }

    void InstanceMemoryManager::updateInstanceDescUploadBuffer(ComPtr<ID3D12Device5> device, 
        MeshMemoryManager& meshMemory, VolumeMemoryManager& volumeMemoryManager)
    {
        // TO_DO Utiliser des buffers upload different par rapport aux frames

        vector<D3D12_RAYTRACING_INSTANCE_DESC> instancesDesc;
        instancesDesc.resize(getNbInstances());

        uint32_t i = 0;

        uint32_t nbSubMeshes = meshMemory.getNbSubMeshes();

        for(Instance& instance : iterateOverInstances())
        {
            XMFLOAT3X4 transform3x4;
            XMStoreFloat3x4(&transform3x4, instance.m_transform);
            memcpy(instancesDesc[i].Transform, &transform3x4, sizeof(instancesDesc[i].Transform));

            instancesDesc[i].InstanceID = i;
            instancesDesc[i].InstanceMask = 0XFF;
            const uint32_t geometryIndex = instance.m_geometryReference.m_geometryIndex;

            if(instance.m_geometryReference.m_type == GeometryType::TRIANGLES)
            {
                instancesDesc[i].InstanceContributionToHitGroupIndex = 
                    meshMemory.get(geometryIndex).m_subMeshFirstIndex;
                instancesDesc[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
                instancesDesc[i].AccelerationStructure =
                    meshMemory.get(geometryIndex).m_blasBuffer.getBuffer()->GetGPUVirtualAddress();
            }
            else if(instance.m_geometryReference.m_type == GeometryType::AABB)
            {
                instancesDesc[i].InstanceContributionToHitGroupIndex = 
                    volumeMemoryManager.get(geometryIndex).m_sbtBaseIndex + nbSubMeshes;
                instancesDesc[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
                instancesDesc[i].AccelerationStructure =
                    volumeMemoryManager.get(geometryIndex).
                    m_blasBuffer.getBuffer()->GetGPUVirtualAddress();
            }

            i++;
        }

        uint64_t instancesDescSize = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * getNbInstances();

        if(instancesDescSize > m_instanceDescUploadBuffer.getSize())
        {
            // TO_DO Ajouter un gestionnaire de upload buffer
            throwIfFailed(true == false, "Unimplemented");
            m_instanceDescUploadBuffer.reallocate(device, instancesDescSize);
        }

        m_instanceDescUploadBuffer.upload(instancesDesc.data(), instancesDescSize);
    }

    void InstanceMemoryManager::updateResizedTlas(
        ComPtr<ID3D12Device5> device, ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
        inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        inputs.NumDescs = getNbInstances();
        inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputs.InstanceDescs = m_instanceDescUploadBuffer.getBuffer()->GetGPUVirtualAddress();
        inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO tlasPrebuildInfo = {};
        device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &tlasPrebuildInfo);

        uint64_t scratchSize = alignData(tlasPrebuildInfo.ScratchDataSizeInBytes,
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
        uint64_t tlasSize = alignData(tlasPrebuildInfo.ResultDataMaxSizeInBytes,
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

        if (m_tlasBuffer.getSize() < tlasSize)
        {
            m_tlasBuffer.reallocate(device, tlasSize,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 
                D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
        }
            
        if (m_scratchTlasBuffer.getSize() < scratchSize)
        {
            m_scratchTlasBuffer.reallocate(device, scratchSize,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_scratchTlasBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            directCommandList->ResourceBarrier(1, &barrier);
        }

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
        desc.Inputs = inputs;
        desc.ScratchAccelerationStructureData = m_scratchTlasBuffer.getBuffer()->GetGPUVirtualAddress();
        desc.DestAccelerationStructureData = m_tlasBuffer.getBuffer()->GetGPUVirtualAddress();

        directCommandList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

        CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(
            m_tlasBuffer.getBuffer().Get());
        directCommandList->ResourceBarrier(1, &uavBarrier);
    }

    void InstanceMemoryManager::updateTransformedTlas(
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
        inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
        inputs.NumDescs = getNbInstances();
        inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        inputs.InstanceDescs = m_instanceDescUploadBuffer.getBuffer()->GetGPUVirtualAddress();
        inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE |
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};
        desc.Inputs = inputs;
        desc.ScratchAccelerationStructureData = m_scratchTlasBuffer.getBuffer()->GetGPUVirtualAddress();
        desc.DestAccelerationStructureData = m_tlasBuffer.getBuffer()->GetGPUVirtualAddress();
        desc.SourceAccelerationStructureData = m_tlasBuffer.getBuffer()->GetGPUVirtualAddress();

        directCommandList->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);

        CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(
            m_tlasBuffer.getBuffer().Get());
        directCommandList->ResourceBarrier(1, &uavBarrier);
    }

    PhysicalIndex InstanceMemoryManager::add(const Instance& instance, const LogicalIndex instanceIndex)
    {
        uint32_t physicalInstanceIndex = m_instancesPool.alloc();
        m_logicalIndexToPhysical[instanceIndex.m_index] = physicalInstanceIndex;
        m_instancesPool.get<Instance>(physicalInstanceIndex) = instance;

        m_hasTlasSizeChanged = true;

        return { physicalInstanceIndex };
    }

    void InstanceMemoryManager::free(const LogicalIndex instanceIndex)
    {
        m_instancesPool.free(m_logicalIndexToPhysical[instanceIndex.m_index]);
        m_logicalIndexToPhysical[instanceIndex.m_index] = UINT32_MAX;
        m_hasTlasSizeChanged = true;
    }

    void InstanceMemoryManager::modify(const LogicalIndex instanceIndex, const Instance& instance)
    {
        m_instancesPool.get<Instance>(m_logicalIndexToPhysical[instanceIndex.m_index]) = instance;
        m_hastTlasTransformed = true;
    }
}


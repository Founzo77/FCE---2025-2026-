#include <fge/render/pipeline_state/SbtTable.hpp>
#include <fge/render/pipeline_state/CbvSrvUavView.hpp>
#include <fge/render/pipeline_state/ShaderBank.hpp>

#include <fge/render/memory/SceneMemoryManager.hpp>
#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/VolumeData.hpp>
#include <fge/render/data/Volume.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void SbtTable::initialize(ComPtr<ID3D12Device5> device, ShaderBank& shaderBank,
        const CbvSrvUavView& handleView, const SceneMemoryManager& sceneMemory)
    {
        constexpr uint32_t shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        m_shaderIdentifierSize = shaderIdentifierSize;

        constexpr uint32_t surfaceHandleCount = 2;
        constexpr uint32_t surfaceRecordSizeRaw = shaderIdentifierSize + 
            surfaceHandleCount * sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) + sizeof(SubMeshData);

        constexpr uint32_t volumeRecordSizeRaw = shaderIdentifierSize + sizeof(VolumeData);

        const uint32_t hitRecordStride = alignData(std::max(surfaceRecordSizeRaw, volumeRecordSizeRaw), 
            D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

        uint32_t nbSurfaceRecords = sceneMemory.getNbSubMeshes();
        uint32_t nbVolumeRecords = sceneMemory.getNbVolumes();

        m_nbHitGroupTables = nbSurfaceRecords + nbVolumeRecords;

        const uint32_t shaderTableSize =
            alignData(hitRecordStride, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

        uint32_t sbtSize =
            shaderTableSize + // raygen
            shaderTableSize + // miss
            shaderTableSize * m_nbHitGroupTables;
    
        m_sbtBuffer.initialize(device, sbtSize);

        m_hitGroupStride = hitRecordStride;
        m_shaderTableSize = shaderTableSize;

        uint8_t* uploadAddress = reinterpret_cast<uint8_t*>(m_sbtBuffer.startUpload());

        memcpy(uploadAddress, shaderBank.getRayGenShaderIdentifier(), shaderIdentifierSize);
        uploadAddress += shaderTableSize;

        memcpy(uploadAddress, shaderBank.getMissShaderIdentifier(), shaderIdentifierSize);
        uploadAddress += shaderTableSize;

        uint32_t indexSubMesh = 0;
        uint32_t recordIndex = 0;

        constexpr uint32_t handleSize = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);

        for(auto meshIndex : sceneMemory.m_meshesMemory.iterateOverIndexMeshes())
        {
            LogicalIndex logicalMeshIndex = 
                sceneMemory.m_meshesMemory.physicalToLogicalIndex(meshIndex.index);
            // TO_DO : Checker sur l'utilisation de logicalMeshIndex
            D3D12_GPU_DESCRIPTOR_HANDLE vertexSrv = 
                handleView.getVertexSrvHandle(logicalMeshIndex.m_index);

            const Mesh& mesh = sceneMemory.getMesh(logicalMeshIndex);

            for(uint32_t k = 0; k < mesh.m_subMeshes.size(); k++)
            {
                uint8_t* record = uploadAddress + recordIndex * hitRecordStride;

                memcpy(record, shaderBank.getHitGroupIdentifier(mesh.m_hitGroupIndex), 
                    shaderIdentifierSize);

                D3D12_GPU_DESCRIPTOR_HANDLE indexSrv = handleView.getIndexSrvHandle(indexSubMesh);

                memcpy(record + shaderIdentifierSize, &vertexSrv, handleSize);
                memcpy(record + shaderIdentifierSize + handleSize, &indexSrv, handleSize);
                
                memcpy(record + shaderIdentifierSize + handleSize * 2, 
                    &sceneMemory.getMesh(logicalMeshIndex).m_subMeshesData[k], sizeof(SubMeshData));
                
                indexSubMesh++;
                recordIndex++;
            }
        }

        for (auto volumeIndex : sceneMemory.m_volumesMemory.iterateOverIndexVolumes())
        {
            LogicalIndex logicalVolumeIndex = 
                sceneMemory.m_volumesMemory.physicalToLogicalIndex(volumeIndex.index);
            const Volume& volume = sceneMemory.getVolume(logicalVolumeIndex);

            uint8_t* record = uploadAddress + recordIndex * hitRecordStride;

            memcpy(record, shaderBank.getHitGroupIdentifier(volume.m_hitGroupIndex), 
                shaderIdentifierSize);
            memcpy(record + shaderIdentifierSize, &volume.m_volumeData, sizeof(VolumeData));

            recordIndex++;
        }


        m_sbtBuffer.endUpload();
    }

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE SbtTable::getRaygenerationShaderRecord()
    {
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE rayGenerationRecord;
        rayGenerationRecord.StartAddress = m_sbtBuffer.getBuffer()->GetGPUVirtualAddress();
        rayGenerationRecord.SizeInBytes = m_shaderIdentifierSize;

        return rayGenerationRecord;
    }

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE SbtTable::getMissShaderTable()
    {
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE missRecord;
        missRecord.StartAddress = 
            m_sbtBuffer.getBuffer()->GetGPUVirtualAddress() + m_shaderTableSize;
        missRecord.SizeInBytes = m_shaderIdentifierSize;
        missRecord.StrideInBytes = m_shaderIdentifierSize;

        return missRecord;
    }

    D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE SbtTable::getHitGroupTable()
    {
        D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE hitRecord;
        hitRecord.StartAddress = 
            m_sbtBuffer.getBuffer()->GetGPUVirtualAddress() + (m_shaderTableSize * 2);
        hitRecord.SizeInBytes = m_hitGroupStride * m_nbHitGroupTables;
        hitRecord.StrideInBytes = m_hitGroupStride;

        return hitRecord;
    }

    void SbtTable::startUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {

    }

    void SbtTable::endUpdateSbt(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {

    }
}
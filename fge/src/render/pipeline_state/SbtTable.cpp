#include <fge/render/pipeline_state/SbtTable.hpp>
#include <fge/render/pipeline_state/CbvSrvUavView.hpp>

#include <fge/render/memory/SceneMemoryManager.hpp>
#include <fge/render/data/Mesh.hpp>

#include <fge/utility.hpp>

namespace fge
{
    void SbtTable::initialize(ComPtr<ID3D12Device5> device, 
        void* rayGenShaderIdentifier, void* missShaderIdentifier, void* hitGroupIdentifier,
        const CbvSrvUavView& handleView, const SceneMemoryManager& sceneMemory)
    {
        constexpr uint32_t shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
        m_shaderIdentifierSize = shaderIdentifierSize;
    
        constexpr uint32_t handleSize = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
        constexpr uint32_t nbHandles = 2;

        constexpr uint32_t constantRootSize = sizeof(uint32_t);
        constexpr uint32_t nbConstantRoot = 1;

        // Taille totale d'un record = identifiant + 2 handles
        const uint32_t rawRecordSize = shaderIdentifierSize + nbHandles * handleSize
            + nbConstantRoot * constantRootSize;
        const uint32_t shaderRecordSize = alignData(rawRecordSize, 
            D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT); // aligné 32
        const uint32_t shaderStride = shaderRecordSize;
        m_hitGroupStride = shaderRecordSize;

        m_shaderTableSize = alignData(shaderRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

        m_rayGenShaderIdentifier = rayGenShaderIdentifier;
        m_missShaderIdentifier = missShaderIdentifier;
        m_hitGroupIdentifier = hitGroupIdentifier;

        const uint32_t shaderTableSize = alignData(
            shaderRecordSize, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

        uint32_t nbSubMeshes = sceneMemory.getNbMaxSubMeshes();
        m_nbHitGroupTables = nbSubMeshes;

        uint32_t sbtSize = shaderTableSize * 2 + shaderTableSize * nbSubMeshes;

        m_sbtBuffer.initialize(device, sbtSize);

        uint8_t* uploadAddress = reinterpret_cast<uint8_t*>(m_sbtBuffer.startUpload());

        const void* data[3] = { 
            rayGenShaderIdentifier, missShaderIdentifier, hitGroupIdentifier
        };

        memcpy(uploadAddress, data[0], shaderIdentifierSize);
        uploadAddress += shaderTableSize;

        memcpy(uploadAddress, data[1], shaderIdentifierSize);
        uploadAddress += shaderTableSize;

        uint32_t indexSubMesh = 0;

        for(auto meshIndex : sceneMemory.m_meshesMemory.iterateOverIndexMeshes())
        {
            LogicalIndex logicalMeshIndex = 
                sceneMemory.m_meshesMemory.physicalToLogicalIndex(meshIndex.index);
            // TO_DO : Checker sur l'utilisation de logicalMeshIndex
            D3D12_GPU_DESCRIPTOR_HANDLE vertexSrv = 
                handleView.getVertexSrvHandle(logicalMeshIndex.m_index);

            for(uint32_t k = 0; k < sceneMemory.getMesh(logicalMeshIndex).m_subMeshes.size(); k++)
            {
                memcpy(uploadAddress, data[2], shaderIdentifierSize);

                D3D12_GPU_DESCRIPTOR_HANDLE indexSrv = handleView.getIndexSrvHandle(indexSubMesh);

                memcpy(uploadAddress + shaderIdentifierSize, &vertexSrv, handleSize);
                memcpy(uploadAddress + shaderIdentifierSize + handleSize, &indexSrv, handleSize);
                
                memcpy(uploadAddress + shaderIdentifierSize + handleSize * 2, 
                    &sceneMemory.getMesh(logicalMeshIndex).m_subMeshesData[k], sizeof(SubMeshData));
                
                uploadAddress += shaderStride;

                indexSubMesh++;
            }
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
        hitRecord.SizeInBytes = m_shaderTableSize * m_nbHitGroupTables;
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
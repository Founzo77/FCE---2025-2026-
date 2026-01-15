#include <fge/render/memory/MeshMemoryManager.hpp>
#include <fge/render/data/Mesh.hpp>

#include <fge/utility.hpp>

#include <d3dx12.h>

namespace fge
{
    void MeshMemoryManager::startInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_meshesPool.initialize(getNbMaxMeshes(), getMeshPageSize());
        m_subMeshesIndexPool.initialize(getNbMaxSubMeshes());

        m_nbSubMeshes = 0;

        m_logicalIndexToPhysical.resize(getNbMaxMeshes(), UINT32_MAX);
        m_physicalIndexToLogical.resize(getNbMaxMeshes(), UINT32_MAX);
    }

    void MeshMemoryManager::moveAllToDevice(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        m_maxBlasScratchSize = 0;
        m_nbSubMeshes = 0;

        for(Mesh& mesh : iterateOverMeshes())
        {
            DefaultDeviceBuffer vertexBuffer;
            DefaultDeviceBuffer indexBuffer;
            DefaultDeviceBuffer blasBuffer;

            uint64_t vertexBufferSize = sizeof(Vertex) * mesh.m_vertices.size();
            uint64_t indexBufferSize = sizeof(uint32_t) * mesh.m_indices.size();

            vertexBuffer.initialize(device, vertexBufferSize, D3D12_RESOURCE_FLAG_NONE);
            indexBuffer.initialize(device, indexBufferSize, D3D12_RESOURCE_FLAG_NONE);

            CD3DX12_RESOURCE_BARRIER barriers[2] = {
                CD3DX12_RESOURCE_BARRIER::Transition(
                    vertexBuffer.getBuffer().Get(),
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
                CD3DX12_RESOURCE_BARRIER::Transition(
                    indexBuffer.getBuffer().Get(),
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
            };
            directCommandList->ResourceBarrier(_countof(barriers), barriers);

            UploadBuffer vertexUploadBuffer;
            UploadBuffer indexUploadBuffer;

            vertexUploadBuffer.initialize(device, vertexBufferSize);
            indexUploadBuffer.initialize(device, indexBufferSize);

            vertexUploadBuffer.upload(mesh.m_vertices.data(), vertexBufferSize);
            indexUploadBuffer.upload(mesh.m_indices.data(), indexBufferSize);

            directCommandList->CopyBufferRegion(vertexBuffer.getBuffer().Get(),
                0, vertexUploadBuffer.getBuffer().Get(), 0, vertexBufferSize);
            directCommandList->CopyBufferRegion(indexBuffer.getBuffer().Get(),
                0, indexUploadBuffer.getBuffer().Get(), 0, indexBufferSize);

            mesh.m_vertexUploadBuffer = std::move(vertexUploadBuffer);
            mesh.m_indexUploadBuffer = std::move(indexUploadBuffer);

            barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
                vertexBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
            barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
                indexBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            directCommandList->ResourceBarrier(_countof(barriers), barriers);
            
            // TO_DO Voir si ce n'est pas mieux de pas mettre Vertex mais juste 
            // Vertex avec uniquement la position et pas l'UV

            std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geometryDescs;
            geometryDescs.reserve(mesh.m_subMeshes.size());

            mesh.m_subMeshesData.clear();

            for(const SubMesh& submesh : mesh.m_subMeshes)
            {
                D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
                geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
                geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

                geometryDesc.Triangles.VertexBuffer.StartAddress =
                    vertexBuffer.getBuffer()->GetGPUVirtualAddress();
                geometryDesc.Triangles.VertexCount = static_cast<UINT>(mesh.m_vertices.size());
                geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
                geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

                geometryDesc.Triangles.IndexBuffer =
                    indexBuffer.getBuffer()->GetGPUVirtualAddress() + 
                    submesh.m_startIndices * sizeof(uint32_t);
                geometryDesc.Triangles.IndexCount = submesh.m_nbIndices;
                geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
                geometryDesc.Triangles.Transform3x4 = 0;

                geometryDescs.push_back(geometryDesc);

                SubMeshData subMeshData;
                subMeshData.m_materialIndex = submesh.m_materialIndex;
                mesh.m_subMeshesData.push_back(subMeshData);
            }

            m_nbSubMeshes += mesh.m_subMeshes.size();
            
            // TO_DO : fusionner les barrieres avec celles des vertices/indices
            uint64_t subMeshesDataBufferSize = sizeof(SubMeshData) * mesh.m_subMeshes.size();

            mesh.m_subMeshesDataBuffer.initialize(
                device, subMeshesDataBufferSize, D3D12_RESOURCE_FLAG_NONE);
            barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
                    mesh.m_subMeshesDataBuffer.getBuffer().Get(),
                    D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
            directCommandList->ResourceBarrier(1, barriers);

            mesh.m_subMeshesDataUploadBuffer.initialize(device, subMeshesDataBufferSize);
            mesh.m_subMeshesDataUploadBuffer.upload(mesh.m_subMeshesData.data(), subMeshesDataBufferSize);
            directCommandList->CopyBufferRegion(mesh.m_subMeshesDataBuffer.getBuffer().Get(),
                0, mesh.m_subMeshesDataUploadBuffer.getBuffer().Get(), 0, subMeshesDataBufferSize);

            barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
                mesh.m_subMeshesDataBuffer.getBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            directCommandList->ResourceBarrier(1, barriers);

            mesh.m_geometryDescs = std::move(geometryDescs);

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInput = {};
            blasInput.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
            blasInput.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            blasInput.NumDescs = static_cast<UINT>(mesh.m_geometryDescs.size());;
            blasInput.pGeometryDescs = mesh.m_geometryDescs.data();
            blasInput.Flags = 
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
            device->GetRaytracingAccelerationStructurePrebuildInfo(&blasInput, &prebuildInfo);
            
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildBlasDesc = {};
            buildBlasDesc.Inputs = blasInput;

            m_maxBlasScratchSize = max(m_maxBlasScratchSize, prebuildInfo.ScratchDataSizeInBytes);

            uint64_t blasSize = alignData(prebuildInfo.ResultDataMaxSizeInBytes,
                D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

            blasBuffer.initialize(device, blasSize, 
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);
            
            mesh.m_blasDescription = std::move(buildBlasDesc);

            mesh.m_vertexBuffer = std::move(vertexBuffer);
            mesh.m_indexBuffer = std::move(indexBuffer);
            mesh.m_blasBuffer = std::move(blasBuffer);
        }

        m_scratchBlasBuffers.initialize(device, m_maxBlasScratchSize, 
            D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
        m_scratchBlasBuffers.allocate(device, m_maxBlasScratchSize, 
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

        for(Mesh& mesh : iterateOverMeshes())
        {
            mesh.m_blasDescription.ScratchAccelerationStructureData = 
                m_scratchBlasBuffers.getBuffer(0)->GetGPUVirtualAddress();
            mesh.m_blasDescription.DestAccelerationStructureData = 
                mesh.m_blasBuffer.getBuffer()->GetGPUVirtualAddress();

            directCommandList->BuildRaytracingAccelerationStructure(&mesh.m_blasDescription,
                0, nullptr);
            CD3DX12_RESOURCE_BARRIER uavBarrier =
                CD3DX12_RESOURCE_BARRIER::UAV(mesh.m_blasBuffer.getBuffer().Get());
            directCommandList->ResourceBarrier(1, &uavBarrier);
        }
    }

    void MeshMemoryManager::endInitialize(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {

    }

    uint64_t MeshMemoryManager::getMeshPageSize() const
    {
        return sizeof(Mesh);
    }

    uint64_t MeshMemoryManager::getSubMeshDataPageSize() const
    {
        return sizeof(SubMeshData);
    }

    uint64_t MeshMemoryManager::getNbMaxMeshes() const
    {
        return 100;
    }

    uint64_t MeshMemoryManager::getNbMaxSubMeshes() const
    {
        return 1000;
    }

    uint32_t MeshMemoryManager::getNbMeshes() const
    {
        return m_meshesPool.getNbElements();
    }

    uint32_t MeshMemoryManager::getNbSubMeshes() const
    {
        return m_nbSubMeshes;
    }

    PhysicalIndex MeshMemoryManager::logicalToPhysicalIndex(const LogicalIndex logicalIndex) const
    {
        return { m_logicalIndexToPhysical[logicalIndex.m_index] };
    }

    LogicalIndex MeshMemoryManager::physicalToLogicalIndex(const PhysicalIndex physicalIndex) const
    {
        return { m_physicalIndexToLogical[physicalIndex.m_index] };
    }

    Mesh& MeshMemoryManager::get(const LogicalIndex meshIndex)
    {
        return m_meshesPool.get<Mesh>(m_logicalIndexToPhysical[meshIndex.m_index]);
    }

    const Mesh& MeshMemoryManager::get(const LogicalIndex meshIndex) const
    {
        return m_meshesPool.get<Mesh>(m_logicalIndexToPhysical[meshIndex.m_index]);
    }

    SafePageBasedAllocator::OccupiedElementRange<Mesh> MeshMemoryManager::iterateOverMeshes()
    {
        return m_meshesPool.occupiedElements<Mesh>();
    }

    SafePageBasedAllocator::IndexedOccupiedElementRange<Mesh> 
        MeshMemoryManager::iterateOverIndexMeshes()
    {
        return m_meshesPool.indexedOccupiedElements<Mesh>();
    }

    SafePageBasedAllocator::IndexedOccupiedElementRange<const Mesh> 
        MeshMemoryManager::iterateOverIndexMeshes() const
    {
        return m_meshesPool.indexedOccupiedElements<const Mesh>();
    }

    PhysicalIndex MeshMemoryManager::add(Mesh&& mesh, const LogicalIndex meshIndex)
    {
        uint32_t physicalMeshIndex = m_meshesPool.alloc();
        m_logicalIndexToPhysical[meshIndex.m_index] = physicalMeshIndex;
        m_physicalIndexToLogical[physicalMeshIndex] = meshIndex.m_index;
        Mesh& currentMesh = m_meshesPool.get<Mesh>(physicalMeshIndex);
        currentMesh = std::move(mesh);
        currentMesh.m_subMeshFirstIndex = 
            m_subMeshesIndexPool.alloc(currentMesh.m_subMeshes.size());

        return { physicalMeshIndex };
    }

    void MeshMemoryManager::share(const LogicalIndex meshIndex)
    {
        m_meshesPool.share(m_logicalIndexToPhysical[meshIndex.m_index]);
    }

    bool MeshMemoryManager::free(const LogicalIndex meshIndex)
    {
        bool has_freed = m_meshesPool.free(m_logicalIndexToPhysical[meshIndex.m_index]);

        if(has_freed)
        {
            m_physicalIndexToLogical[m_logicalIndexToPhysical[meshIndex.m_index]] = UINT32_MAX;
            m_logicalIndexToPhysical[meshIndex.m_index] = UINT32_MAX;
        }

        return has_freed;
        // TO_DO Gerer : m_nbSubMeshes et m_subMeshFirstIndex
    }
}


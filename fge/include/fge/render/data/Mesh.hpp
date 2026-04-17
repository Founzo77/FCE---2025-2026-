#pragma once

#include "Vertex.hpp"
#include "SubMesh.hpp"
#include "SubMeshData.hpp"
#include "../memory/UploadBuffer.hpp"
#include "../memory/DefaultDeviceBuffer.hpp"

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>
#include <cstdint>

using namespace Microsoft::WRL;
using std::vector;

namespace fge
{
    class Mesh
    {
    public:
        vector<D3D12_RAYTRACING_GEOMETRY_DESC> m_geometryDescs;
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC m_blasDescription;
        vector<SubMeshData> m_subMeshesData;
        UploadBuffer m_vertexUploadBuffer;
        UploadBuffer m_indexUploadBuffer;
        UploadBuffer m_subMeshesDataUploadBuffer;
        DefaultDeviceBuffer m_blasBuffer;
        DefaultDeviceBuffer m_vertexBuffer;
        DefaultDeviceBuffer m_indexBuffer;
        DefaultDeviceBuffer m_subMeshesDataBuffer;
        vector<SubMesh> m_subMeshes;
        vector<Vertex> m_vertices;
        vector<uint32_t> m_indices;
        uint32_t m_subMeshFirstIndex;
        uint32_t m_hitGroupIndex;

    public:
        Mesh() = default;
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh(Mesh&&) = default;

        Mesh& operator=(const Mesh&) = delete;
        Mesh& operator=(Mesh&&) = default;

        void reset();
    };
}
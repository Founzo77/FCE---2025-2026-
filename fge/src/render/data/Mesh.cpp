#include <fge/render/data/Mesh.hpp>

namespace fge
{
    Mesh::~Mesh()
    {
        reset();
    }

    void Mesh::reset()
    {
        m_geometryDescs.clear();
        m_geometryDescs.shrink_to_fit();

        m_blasDescription = {};

        m_subMeshesData.clear();
        m_subMeshesData.shrink_to_fit();

        m_vertexUploadBuffer.reset();
        m_indexUploadBuffer.reset();
        m_subMeshesDataUploadBuffer.reset();

        m_blasBuffer.reset();
        m_vertexBuffer.reset();
        m_indexBuffer.reset();
        m_subMeshesDataBuffer.reset();

        m_subMeshes.clear();
        m_subMeshes.shrink_to_fit();

        m_vertices.clear();
        m_vertices.shrink_to_fit();

        m_indices.clear();
        m_indices.shrink_to_fit();

        m_subMeshFirstIndex = 0;
        m_hitGroupIndex = 0;
    }
}
#include <fgewa/render/data/Geometry.hpp>
#include <fgewa/render/data/Device.hpp>

#include <fge/render/data/Mesh.hpp>
#include <fge/utility.hpp>

using fge::throwIfFailed;

namespace fgewa
{
    Geometry::~Geometry()
    {
        reset();
        m_device = nullptr;
    }

    void Geometry::initialize(shared_ptr<Device> device, const fge::Mesh& meshData)
    {
        m_device = device;
        throwIfFailed(!m_geometry, "A Geometry has already been created");

        m_geometry = anariNewGeometry(m_device->getHandle(), "triangle");

        throwIfFailed(!m_geometry == false, "Failed to create ANARI Geometry");

        // 2️⃣ Extraire les tableaux
        const auto &vertices = meshData.m_vertices;
        const auto &indices = meshData.m_indices;

        // Préparer tableaux contigus CPU
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> uvs;

        positions.reserve(vertices.size() * 3);
        normals.reserve(vertices.size() * 3);
        uvs.reserve(vertices.size() * 2);

        for (const auto &v : vertices)
        {
            positions.push_back(v.m_position.x);
            positions.push_back(v.m_position.y);
            positions.push_back(v.m_position.z);

            normals.push_back(v.m_normal.x);
            normals.push_back(v.m_normal.y);
            normals.push_back(v.m_normal.z);

            uvs.push_back(v.m_uv.x);
            uvs.push_back(v.m_uv.y);
        }

        ANARIArray1D positionArray = anariNewArray1D(
            m_device->getHandle(), positions.data(),
            nullptr, 0, ANARI_FLOAT32_VEC3, positions.size() / 3);

        ANARIArray1D normalArray = anariNewArray1D(
            m_device->getHandle(), normals.data(), 0,
            nullptr, ANARI_FLOAT32_VEC3, normals.size() / 3);

        ANARIArray1D texArray = anariNewArray1D(
            m_device->getHandle(), uvs.data(), 0,
            nullptr, ANARI_FLOAT32_VEC2, uvs.size() / 2);

        ANARIArray1D indexArray = anariNewArray1D(
            m_device->getHandle(), indices.data(), 0,
            nullptr, ANARI_UINT32_VEC3, indices.size() / 3);

        anariSetParameter(m_device->getHandle(), m_geometry,
            "vertex.position", ANARI_ARRAY1D, &positionArray);

        anariSetParameter(m_device->getHandle(), m_geometry,
            "vertex.normal", ANARI_ARRAY1D, &normalArray);

        anariSetParameter(m_device->getHandle(), m_geometry,
            "vertex.texcoord", ANARI_ARRAY1D, &texArray);

        anariSetParameter(m_device->getHandle(), m_geometry,
            "primitive.index", ANARI_ARRAY1D, &indexArray);

        anariCommitParameters(m_device->getHandle(), m_geometry);

        anariRelease(m_device->getHandle(), positionArray);
        anariRelease(m_device->getHandle(), normalArray);
        anariRelease(m_device->getHandle(), texArray);
        anariRelease(m_device->getHandle(), indexArray);
    }

    void Geometry::reset()
    {
        if(m_geometry)
        {
            anariRelease(m_device->getHandle(), m_geometry);
            m_geometry = nullptr;
        }
    }

    ANARIGeometry Geometry::getHandle() noexcept
    {
        return m_geometry;
    }

    const ANARIGeometry Geometry::getHandle() const noexcept
    {
        return m_geometry;
    }

    ANARIGeometry* Geometry::getMemoryHandle() noexcept
    {
        return &m_geometry;
    }
}
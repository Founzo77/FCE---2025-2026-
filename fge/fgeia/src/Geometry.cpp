#include <fgeia/Geometry.hpp>

#include <fge/render/data/Mesh.hpp>
#include <fge/render/pipeline_state/ShaderBank.hpp>

namespace fgeia
{
    Geometry::Geometry(FgeGlobalState* globalState) : Object(ANARI_GEOMETRY, globalState)
    {

    }

    Geometry* Geometry::createInstance(std::string_view subtype, FgeGlobalState* globalState)
    {
        if (subtype == "triangle")
            return new Triangle(globalState);
        return (Geometry *)new UnknownObject(ANARI_GEOMETRY, subtype, globalState);
    }

    fge::LogicalIndex Geometry::getFgeMeshIndex() const
    {
        return m_fgeMeshIndex;
    }

    void Geometry::commitParameters()
    {
        anari::math::float4 invalidAttr(NAN, NAN, NAN, NAN);

        m_constantAttributes[0] = getParam<anari::math::float4>("attribute0", invalidAttr);
        m_constantAttributes[1] = getParam<anari::math::float4>("attribute1", invalidAttr);
        m_constantAttributes[2] = getParam<anari::math::float4>("attribute2", invalidAttr);
        m_constantAttributes[3] = getParam<anari::math::float4>("attribute3", invalidAttr);
        m_constantAttributes[4] = getParam<anari::math::float4>("color", invalidAttr);

        m_primitiveAttributes[0] = getParamObject<Array1D>("primitive.attribute0");
        m_primitiveAttributes[1] = getParamObject<Array1D>("primitive.attribute1");
        m_primitiveAttributes[2] = getParamObject<Array1D>("primitive.attribute2");
        m_primitiveAttributes[3] = getParamObject<Array1D>("primitive.attribute3");
        m_primitiveAttributes[4] = getParamObject<Array1D>("primitive.color");

        m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
        m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
        m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
        m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
        m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
    }

    void Geometry::finalize()
    {
        if(m_fgeMeshIndex == UINT32_MAX)
        {
            m_fgeMeshIndex = globalState()->m_meshIndexAllocator.alloc();
        }
    }

    Triangle::Triangle(FgeGlobalState* globalState) : Geometry(globalState),
        m_index(this), m_vertexPosition(this), m_vertexNormal(this)
    {
    
    }

    void Triangle::commitParameters()
    {
        Geometry::commitParameters();
        m_index = getParamObject<Array1D>("primitive.index");
        m_vertexPosition = getParamObject<Array1D>("vertex.position");
        m_vertexNormal = getParamObject<Array1D>("vertex.normal");
        m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
        m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
        m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
        m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
        m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
        m_faceVaryingAttributes[0] = getParamObject<Array1D>("faceVarying.attribute0");
        m_faceVaryingAttributes[1] = getParamObject<Array1D>("faceVarying.attribute1");
        m_faceVaryingAttributes[2] = getParamObject<Array1D>("faceVarying.attribute2");
        m_faceVaryingAttributes[3] = getParamObject<Array1D>("faceVarying.attribute3");
        m_faceVaryingAttributes[4] = getParamObject<Array1D>("faceVarying.color");
        m_faceVaryingAttributes[5] = getParamObject<Array1D>("faceVarying.normal");
    }

    void Triangle::finalize()
    {
        if (!m_vertexPosition) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, 
                "missing required parameter 'vertex.position' on triangle geometry");
            return;
        }

        Geometry::finalize();

        fge::Mesh fgeMesh;

        // vertices
        const uint32_t numVertices = (uint32_t)m_vertexPosition->size();

        const anari::math::float3* positions = m_vertexPosition->dataAs<anari::math::float3>();

        const anari::math::float3* normals = nullptr;

        if (m_vertexNormal)
            normals = m_vertexNormal->dataAs<anari::math::float3>();

        const anari::math::float2* uvs = nullptr;

        if (m_vertexAttributes[0])
            uvs = m_vertexAttributes[0]->dataAs<anari::math::float2>();

        fgeMesh.m_vertices.reserve(numVertices);

        for (uint32_t i = 0; i < numVertices; ++i)
        {
            XMFLOAT3 pos = {positions[i].x, positions[i].y, positions[i].z};

            XMFLOAT3 nrm = normals ?
                XMFLOAT3(normals[i].x, normals[i].y, normals[i].z) : XMFLOAT3(0.f, 0.f, 1.f);

            XMFLOAT2 uv = uvs ? XMFLOAT2(uvs[i].x, uvs[i].y) : XMFLOAT2(0.f, 0.f);

            fgeMesh.m_vertices.emplace_back(pos, nrm, uv);
        }

        // indices
        if (m_index)
        {
            uint32_t numTriangles = (uint32_t)m_index->size();

            fgeMesh.m_indices.reserve(numTriangles * 3);

            if (m_index->elementType() == ANARI_UINT32_VEC3)
            {
                const anari::math::uint3* tri = m_index->dataAs<anari::math::uint3>();

                for (uint32_t i = 0; i < numTriangles; ++i)
                {
                    fgeMesh.m_indices.push_back(tri[i].x);
                    fgeMesh.m_indices.push_back(tri[i].y);
                    fgeMesh.m_indices.push_back(tri[i].z);
                }
            }
            else
            {
                reportMessage(ANARI_SEVERITY_ERROR, "primitive.index has unsupported element type");
            }
        }
        else
        {
            // générer indices si non fournis
            const uint32_t numIndices = numVertices;

            fgeMesh.m_indices.resize(numIndices);

            for (uint32_t i = 0; i < numIndices; ++i)
                fgeMesh.m_indices[i] = i;
        }

        // submesh
        fgeMesh.m_subMeshes.emplace_back(0, (uint32_t)fgeMesh.m_indices.size(), 0);

        // upload data metadata

        fgeMesh.m_subMeshFirstIndex = 0;
        fgeMesh.m_hitGroupIndex = fge::ShaderBank::getHitGroupIndex(L"PhongHitGroup");

        globalState()->setMesh(std::move(fgeMesh), m_fgeMeshIndex);
    }

    bool Triangle::isValid() const
    {
        return m_vertexPosition;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Geometry*);
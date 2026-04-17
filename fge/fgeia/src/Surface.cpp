#include <fgeia/Surface.hpp>

#include <fge/render/data/Mesh.hpp>

namespace fgeia
{
    Surface::Surface(FgeGlobalState* globalState) : 
        Object(ANARI_SURFACE, globalState), m_geometry(this), m_material(this)
    {
        
    }

    fge::LogicalIndex Surface::getFgeMeshIndex() const
    {
        return m_geometry->getFgeMeshIndex();
    }

    void Surface::commitParameters()
    {
        m_geometry = getParamObject<Geometry>("geometry");
        m_material = getParamObject<Material>("material");
    }

    void Surface::finalize()
    {
        if (!m_material) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, "missing 'material' on ANARISurface");
            return;
        }

        if (!m_geometry) 
        {
            reportMessage(ANARI_SEVERITY_WARNING, "missing 'geometry' on ANARISurface");
            return;
        }

        fge::LogicalIndex fgeMeshIndex = getFgeMeshIndex();
        fge::LogicalIndex fgeMaterialIndex = m_material->getFgeIndex();

        fge::Mesh& fgeMesh = 
            globalState()->getSceneDescription().m_meshes.at(fgeMeshIndex.m_index);
        fgeMesh.m_subMeshes[0].m_materialIndex = fgeMaterialIndex.m_index;
    }

    bool Surface::isValid() const
    {
        return m_geometry && m_material && m_geometry->isValid() && m_material->isValid();
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Surface*);

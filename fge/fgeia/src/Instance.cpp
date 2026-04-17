#include <fgeia/Instance.hpp>

namespace fgeia
{
    Instance::Instance(FgeGlobalState* globalState) : Object(ANARI_INSTANCE, globalState)
    {

    }

    const std::vector<fge::LogicalIndex>& Instance::getFgeInstancesIndex() const
    {
        return m_fgeInstancesIndex;
    }

    void Instance::commitParameters()
    {
        anari::math::mat4 xfm = anari::math::identity;
        getParam("transform", ANARI_FLOAT32_MAT4, &xfm);
        
        m_transform = xfm;
        m_group = getParamObject<Group>("group");
    }

    void Instance::finalize()
    {
        if (!m_group)
            reportMessage(ANARI_SEVERITY_WARNING, "missing 'group' on ANARIInstance");

        // TO_DO 
        
        // convert transform
        const float *m = (const float *)&m_transform;
        XMMATRIX transform = XMMATRIX(
            m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]);

        // Surfaces -> triangle meshes
        if (m_group->m_surfaceData)
        {
            for (auto it = m_group->m_surfaceData->handlesBegin();
                it != m_group->m_surfaceData->handlesEnd(); it++)
            {
                Surface* surface = (Surface*) *it;

                if (!surface || !surface->isValid())
                    continue;

                fge::Instance fgeInstance;

                fgeInstance.m_geometryReference.m_type = fge::GeometryType::TRIANGLES;
                fgeInstance.m_geometryReference.m_geometryIndex = 
                    surface->getFgeMeshIndex().m_index;
                fgeInstance.m_transform = transform;
                fge::LogicalIndex fgeInstanceIndex = 
                    globalState()->m_instanceIndexAllocator.alloc();

                m_fgeInstancesIndex.push_back(fgeInstanceIndex);
                globalState()->setInstance(std::move(fgeInstance), fgeInstanceIndex);
            }
        }

        // Volumes -> AABB geometry
        if (m_group->m_volumeData)
        {
            for (auto it = m_group->m_volumeData->handlesBegin();
                it != m_group->m_volumeData->handlesEnd(); it++)
            {
                Volume* volume = (Volume*) *it;

                if (!volume || !volume->isValid())
                    continue;

                fge::Instance fgeInstance;

                fgeInstance.m_geometryReference.m_type = fge::GeometryType::AABB;
                fgeInstance.m_geometryReference.m_geometryIndex = 
                    volume->getFgeVolumeIndex().m_index;
                fgeInstance.m_transform = transform;
                fge::LogicalIndex fgeInstanceIndex =
                    globalState()->m_instanceIndexAllocator.alloc();

                m_fgeInstancesIndex.push_back(fgeInstanceIndex);
                globalState()->setInstance(std::move(fgeInstance), fgeInstanceIndex);
            }
        }

        // Lights non supportées pour l'instant
    }

    bool Instance::isValid() const
    {
        return m_group;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Instance*);

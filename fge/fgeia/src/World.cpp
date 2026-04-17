#include <fgeia/World.hpp>

namespace fgeia
{
    World::World(FgeGlobalState* globalState) : Object(ANARI_WORLD, globalState),
        m_zeroSurfaceData(this), m_zeroVolumeData(this),
        m_zeroLightData(this), m_instanceData(this)
    {
        m_zeroGroup = new Group(globalState);
        m_zeroInstance = new Instance(globalState);
        m_zeroInstance->setParamDirect("group", m_zeroGroup.ptr);
        m_zeroInstance->commitParameters();
        m_zeroInstance->finalize();

        m_zeroGroup->refDec(helium::RefType::PUBLIC);
        m_zeroInstance->refDec(helium::RefType::PUBLIC);
    }

    bool World::getProperty(const std::string_view& name, ANARIDataType type, 
        void* ptr, uint64_t size, uint32_t flags)
    {
        // TO_DO 
        
        return Object::getProperty(name, type, ptr, size, flags);
    }

    void World::commitParameters()
    {
        m_zeroSurfaceData = getParamObject<helium::ObjectArray>("surface");
        m_zeroVolumeData = getParamObject<helium::ObjectArray>("volume");
        m_zeroLightData = getParamObject<helium::ObjectArray>("light");
        m_instanceData = getParamObject<helium::ObjectArray>("instance");
    }

    void World::finalize()
    {
        finalizeAnariAttributs();
        finalizeFgeAttributs();
    }

    void World::finalizeAnariAttributs()
    {
        const bool addZeroInstance =
            m_zeroSurfaceData || m_zeroVolumeData || m_zeroLightData;

        if (addZeroInstance)
            reportMessage(ANARI_SEVERITY_DEBUG, "FGE::World will add zero instance");

        if (m_zeroSurfaceData) 
        {
            reportMessage(ANARI_SEVERITY_DEBUG, "FGE::World found %zu surfaces in zero instance",
            m_zeroSurfaceData->size());
            m_zeroGroup->setParamDirect("surface", getParamDirect("surface"));
        } 
        else 
        {
            m_zeroGroup->removeParam("surface");
        }

        if (m_zeroVolumeData) 
        {
            reportMessage(ANARI_SEVERITY_DEBUG, "FGE::World found %zu volumes in zero instance",
                m_zeroVolumeData->size());
            m_zeroGroup->setParamDirect("volume", getParamDirect("volume"));
        } 
        else
            m_zeroGroup->removeParam("volume");

        if (m_zeroLightData)
        {
            reportMessage(ANARI_SEVERITY_DEBUG, "FGE::World found %zu lights in zero instance",
                m_zeroLightData->size());
            m_zeroGroup->setParamDirect("light", getParamDirect("light"));
        } 
        else
            m_zeroGroup->removeParam("light");

        m_zeroGroup->commitParameters();
        m_zeroGroup->finalize();

        m_instances.clear();

        if (m_instanceData) 
        {
            std::for_each(m_instanceData->handlesBegin(), m_instanceData->handlesEnd(),
                [&](auto *o) {
                if (o && o->isValid())
                    m_instances.push_back((Instance *)o);
                });
        }

        if (addZeroInstance)
            m_instances.push_back(m_zeroInstance.ptr);
    }

    void World::finalizeFgeAttributs()
    {
        // TO_DO
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::World*);

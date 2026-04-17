#include <fgeia/Group.hpp>

namespace fgeia
{
    Group::Group(FgeGlobalState* globalState) : Object(ANARI_GROUP, globalState),
        m_surfaceData(this), m_volumeData(this), m_lightData(this)
    {

    }

    void Group::commitParameters()
    {
        m_surfaceData = getParamObject<helium::ObjectArray>("surface");
        m_volumeData = getParamObject<helium::ObjectArray>("volume");
        m_lightData = getParamObject<helium::ObjectArray>("light");
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Group*);

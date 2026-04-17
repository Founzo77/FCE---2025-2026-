#pragma once

#include "Object.hpp"
#include "Light.hpp"
#include "Surface.hpp"
#include "Volume.hpp"

#include <helium/array/ObjectArray.h>

namespace fgeia
{
    class Group : public Object
    {
    public:
        helium::ChangeObserverPtr<helium::ObjectArray> m_surfaceData;
        helium::ChangeObserverPtr<helium::ObjectArray> m_volumeData;
        helium::ChangeObserverPtr<helium::ObjectArray> m_lightData;

    public:
        Group(FgeGlobalState* globalState);
        virtual ~Group() override = default;

        void commitParameters() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Group*, ANARI_GROUP);

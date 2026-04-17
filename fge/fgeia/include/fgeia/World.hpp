#pragma once

#include "Object.hpp"
#include "Instance.hpp"
#include "Group.hpp"

#include <helium/array/ObjectArray.h>

namespace fgeia
{
    class World : public Object
    {
    private:
        helium::ChangeObserverPtr<helium::ObjectArray> m_zeroSurfaceData;
        helium::ChangeObserverPtr<helium::ObjectArray> m_zeroVolumeData;
        helium::ChangeObserverPtr<helium::ObjectArray> m_zeroLightData;
        helium::ChangeObserverPtr<helium::ObjectArray> m_instanceData;

        helium::IntrusivePtr<Group> m_zeroGroup;
        helium::IntrusivePtr<Instance> m_zeroInstance;

        std::vector<Instance*> m_instances;

    public:
        World(FgeGlobalState* globalState);
        virtual ~World() override = default;

        bool getProperty(const std::string_view& name,
            ANARIDataType type, void* ptr, uint64_t size, uint32_t flags) override;

        void commitParameters() override;
        void finalize() override;
    private:
        void finalizeAnariAttributs();
        void finalizeFgeAttributs();
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::World*, ANARI_WORLD);

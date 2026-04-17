#pragma once

#include "Object.hpp"
#include "Group.hpp"

#include <vector>

#include <fge/render/indices.hpp>

namespace fgeia
{
    class Instance : public Object
    {
    private:
        anari::math::mat4 m_transform;
        helium::IntrusivePtr<Group> m_group;
        std::vector<fge::LogicalIndex> m_fgeInstancesIndex;

    public:
        Instance(FgeGlobalState* globalState);
        virtual ~Instance() override = default;

        const std::vector<fge::LogicalIndex>& getFgeInstancesIndex() const;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Instance*, ANARI_INSTANCE);

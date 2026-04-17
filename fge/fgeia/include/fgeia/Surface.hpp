#pragma once

#include "Geometry.hpp"
#include "Material.hpp"
#include "Object.hpp"

#include <helium/utility/ChangeObserverPtr.h>

namespace fgeia
{
    class Surface : public Object
    {
    private:
        helium::ChangeObserverPtr<Geometry> m_geometry;
        helium::ChangeObserverPtr<Material> m_material;

    public:
        Surface(FgeGlobalState* globalState);
        virtual ~Surface() override = default;

        fge::LogicalIndex getFgeMeshIndex() const;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Surface*, ANARI_SURFACE);

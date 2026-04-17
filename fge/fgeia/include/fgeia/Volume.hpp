#pragma once

#include "Array.hpp"
#include "Object.hpp"
#include "SpatialField.hpp"

#include <helium/utility/ChangeObserverPtr.h>

#include <fge/render/indices.hpp>

namespace fgeia
{
    class Volume : public Object
    {
    protected:
        fge::LogicalIndex m_fgeVolumeIndex;

    public:
        Volume(FgeGlobalState* globalState);
        virtual ~Volume() override = default;

        fge::LogicalIndex getFgeVolumeIndex() const;

        static Volume* createInstance(std::string_view subtype, FgeGlobalState* globalState);
        
        virtual void finalize() override;
    };

    class TransferFunction1D : public Volume
    {
    private:
        helium::ChangeObserverPtr<SpatialField> m_field;
        helium::box1 m_valueRange{0.f, 1.f};
        float m_unitDistance{1.f};
        float m_densityScale{1.f};
        helium::float4 m_uniformColor{1.f, 1.f, 1.f, 1.f};
        float m_uniformOpacity{1.f};

        helium::ChangeObserverPtr<helium::Array1D> m_colorData;
        helium::ChangeObserverPtr<helium::Array1D> m_opacityData;

    public:
        TransferFunction1D(FgeGlobalState* globalState);
        virtual ~TransferFunction1D() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Volume*, ANARI_VOLUME);

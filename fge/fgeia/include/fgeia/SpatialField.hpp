#pragma once

#include "Object.hpp"

#include <helium/utility/ChangeObserverPtr.h>
#include <helium/array/Array3D.h>

#include <fge/render/data/VolumeData.hpp>
#include <fge/render/indices.hpp>

namespace fgeia
{
    class SpatialField : public Object
    {
    protected:
        fge::VolumeData m_fgeVolumeData;

    public:
        SpatialField(FgeGlobalState* globalState);
        virtual ~SpatialField() override = default;

        static SpatialField* createInstance(std::string_view subtype, 
            FgeGlobalState* globalState);

        fge::VolumeData& getFgeVolumeData() noexcept;
    };

    class StructuredRegularField : public SpatialField
    {
    private:
        helium::ChangeObserverPtr<helium::Array3D> m_data;
        anari::math::uint3 m_dims{0u};
        anari::math::float3 m_origin;
        anari::math::float3 m_spacing;
        fge::LogicalIndex m_fgeTexture3DIndex;

    public:
        StructuredRegularField(FgeGlobalState* globalState);
        virtual ~StructuredRegularField() override = default;

        fge::LogicalIndex getFgeTexture3DIndex() const noexcept;

        void commitParameters() override;
        void finalize() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::SpatialField*, ANARI_SPATIAL_FIELD);

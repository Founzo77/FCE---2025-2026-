#pragma once

#include "utility.hpp"
#include "Object.hpp"

#include <fge/render/data/Light.hpp>
#include <fge/render/indices.hpp>

namespace fgeia
{
    class Light : public Object
    {
    protected:
        anari::math::float3 m_color;
        fge::LogicalIndex m_fgeLightIndex;

    public:
        Light(FgeGlobalState* globalState);
        virtual ~Light() override = default;

        static Light* createInstance(std::string_view subtype, FgeGlobalState* globalState);

        virtual void commitParameters() override;
        virtual void finalize() override;
        
    protected:
    };

    class Point : public Light
    {
    private:
        anari::math::float3 m_position;
        float m_radius;
        float m_intensity;
        float m_power;
        float m_radiance;

    public:
        Point(FgeGlobalState* globalState);
        virtual ~Point() override = default;

        void commitParameters() override;
        void finalize() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Light*, ANARI_LIGHT);

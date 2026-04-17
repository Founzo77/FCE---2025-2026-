#pragma once

#include "Object.hpp"
#include "Sampler.hpp"

#include <fge/render/data/Material.hpp>
#include <fge/render/indices.hpp>

#include <helium/utility/IntrusivePtr.h>

namespace fgeia
{
    class Material : public Object
    {
    protected:
        fge::LogicalIndex m_fgeMaterialIndex;

    public:
        Material(FgeGlobalState* globalState);
        virtual ~Material() override = default;

        static Material* createInstance(std::string_view subtype, FgeGlobalState* globalState);

        const fge::LogicalIndex& getFgeIndex() const noexcept;

        virtual void finalize() override;
    };

    template <typename SCALAR_T>
    struct MaterialParameter
    {
        SCALAR_T value;
        std::string attribute;
        helium::IntrusivePtr<Sampler> sampler;
    };

    class Matte : public Material
    {
    private:
        MaterialParameter<anari::math::float4> m_color;
        MaterialParameter<float> m_opacity;

    public:
        Matte(FgeGlobalState* globalState);
        virtual ~Matte() override = default;

        void commitParameters() override;
        bool isValid() const override;
        void finalize() override;
    };

    class PhysicallyBased : public Material
    {
    private:
        MaterialParameter<anari::math::float4> m_baseColor;
        MaterialParameter<anari::math::float3> m_emissive;
        MaterialParameter<anari::math::float3> m_specularColor;
        MaterialParameter<float> m_opacity;
        MaterialParameter<float> m_metallic;
        MaterialParameter<float> m_roughness;
        MaterialParameter<float> m_specular;
        MaterialParameter<float> m_transmission;
        float m_ior;

    public:
        PhysicallyBased(FgeGlobalState* globalState);
        virtual ~PhysicallyBased() override = default;

        void commitParameters() override;
        bool isValid() const override;
        void finalize() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Material*, ANARI_MATERIAL);

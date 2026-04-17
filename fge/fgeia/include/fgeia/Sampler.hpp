#pragma once

#include "Object.hpp"

#include <helium/array/Array1D.h>
#include <helium/array/Array2D.h>
#include <helium/array/Array3D.h>

#include <fge/render/data/Texture.hpp>
#include <fge/render/indices.hpp>

#include <vector>

namespace fgeia
{
    class Sampler : public Object
    {
    protected:
        std::string m_inAttribute;
        helium::mat4 m_inTransform{anari::math::identity};
        helium::float4 m_inOffset{0.f, 0.f, 0.f, 0.f};
        helium::mat4 m_outTransform{anari::math::identity};
        helium::float4 m_outOffset{0.f, 0.f, 0.f, 0.f};

    public:
        Sampler(FgeGlobalState* globalState);
        virtual ~Sampler() override = default;

        static Sampler* createInstance(std::string_view subtype, FgeGlobalState* globalState);

    protected:
        void commitParameters() override;
    };

    class TextureDataSampler : public Sampler
    {
    protected:
        fge::LogicalIndex m_fgeIndex;

    public:
        TextureDataSampler(FgeGlobalState* globalState);
        virtual ~TextureDataSampler() override = default;

        fge::LogicalIndex getFgeIndex() const;

        void commitParameters() override;
        virtual void finalize() override;
    };

    class Image1D : public TextureDataSampler
    {
    private:
        helium::IntrusivePtr<helium::Array1D> m_image;
        std::string m_wrapMode;

    public:
        Image1D(FgeGlobalState* globalState);
        virtual ~Image1D() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
    
    class Image2D : public TextureDataSampler
    {
    private:
        helium::IntrusivePtr<helium::Array2D> m_image;
        std::string m_wrapMode1;
        std::string m_wrapMode2;

    public:
        Image2D(FgeGlobalState* globalState);
        virtual ~Image2D() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };

    class Image3D : public TextureDataSampler
    {
    private:
        helium::IntrusivePtr<helium::Array3D> m_image;
        std::string m_wrapMode1;
        std::string m_wrapMode2;
        std::string m_wrapMode3;

    public:
        Image3D(FgeGlobalState* globalState);
        virtual ~Image3D() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Sampler*, ANARI_SAMPLER);

#pragma once

#include "Object.hpp"
#include "Array.hpp"

#include <helium/utility/ChangeObserverPtr.h>

#include <fge/render/indices.hpp>

#include <array>
#include <vector>

namespace fgeia
{
    class Geometry : public Object
    {
    protected:
        std::array<helium::float4, 5> m_constantAttributes;
        std::array<helium::IntrusivePtr<Array1D>, 5> m_primitiveAttributes;
        std::array<helium::IntrusivePtr<Array1D>, 5> m_vertexAttributes;
        fge::LogicalIndex m_fgeMeshIndex;

    public:
        Geometry(FgeGlobalState* globalState);
        virtual ~Geometry() override = default;

        static Geometry* createInstance(std::string_view subtype, FgeGlobalState* globalState);

        fge::LogicalIndex getFgeMeshIndex() const;

        virtual void commitParameters() override;
        virtual void finalize() override;
    };

    class Triangle : public Geometry
    {
    private:
        helium::ChangeObserverPtr<Array1D> m_index;
        helium::ChangeObserverPtr<Array1D> m_vertexPosition;
        helium::ChangeObserverPtr<Array1D> m_vertexNormal;
        std::array<helium::IntrusivePtr<Array1D>, 6> m_faceVaryingAttributes;
        std::vector<int> m_generatedIndices;

    public:
        Triangle(FgeGlobalState* globalState);
        virtual ~Triangle() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Geometry*, ANARI_GEOMETRY);

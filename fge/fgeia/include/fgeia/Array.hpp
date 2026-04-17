#pragma once

#include "utility.hpp"
#include "FgeGlobalState.hpp"

#include <helium/array/Array1D.h>
#include <helium/array/Array2D.h>
#include <helium/array/Array3D.h>

namespace fgeia
{
    class Array1D : public helium::Array1D
    {
    public:
        Array1D(FgeGlobalState* globalState, const helium::Array1DMemoryDescriptor& descriptor);
        virtual ~Array1D() override = default;

        //void unmap() override;
    };

    class Array2D : public helium::Array2D
    {
    public:
        Array2D(FgeGlobalState* globalState, const helium::Array2DMemoryDescriptor& descriptor);
        virtual ~Array2D() override = default;

        //void unmap() override;
    };

    class Array3D : public helium::Array3D
    {
    public:
        Array3D(FgeGlobalState* globalState, const helium::Array3DMemoryDescriptor& descriptor);
        virtual ~Array3D() override = default;

        //void unmap() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Array1D*, ANARI_ARRAY1D);
FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Array2D*, ANARI_ARRAY2D);
FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Array3D*, ANARI_ARRAY3D);

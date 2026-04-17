#include <fgeia/Array.hpp>

namespace fgeia
{
    Array1D::Array1D(FgeGlobalState* globalState, 
        const helium::Array1DMemoryDescriptor& descriptor) : 
        helium::Array1D(globalState, descriptor)
    {

    }

    Array2D::Array2D(FgeGlobalState* globalState, 
        const helium::Array2DMemoryDescriptor& descriptor) :
        helium::Array2D(globalState, descriptor)
    {

    }

    Array3D::Array3D(FgeGlobalState* globalState, 
        const helium::Array3DMemoryDescriptor& descriptor) :
        helium::Array3D(globalState, descriptor)
    {

    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Array1D*);
FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Array1D*);
FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Array1D*);

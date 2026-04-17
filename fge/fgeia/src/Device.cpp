#include <fgeia/Device.hpp>

#include <fgeia/Light.hpp>
#include <fgeia/Camera.hpp>
#include <fgeia/Geometry.hpp>
#include <fgeia/SpatialField.hpp>
#include <fgeia/Surface.hpp>
#include <fgeia/Volume.hpp>
#include <fgeia/Material.hpp>
#include <fgeia/Sampler.hpp>
#include <fgeia/Instance.hpp>
#include <fgeia/World.hpp>
#include <fgeia/Frame.hpp>
#include <fgeia/Renderer.hpp>

#include "anari_library_fgeia_queries.h"

namespace fgeia
{
    Device::Device(ANARILibrary lib, const std::string& subtype) : helium::BaseDevice(lib)
    {
        m_state = std::make_unique<FgeGlobalState>(this_device());
    }

    Device::~Device()
    {

    }

    FgeGlobalState* Device::globalState()
    {
        FgeGlobalState* state = (FgeGlobalState*)helium::BaseDevice::m_state.get();
        return state;
    }

    ANARIArray1D Device::newArray1D(const void* appMemory, 
        ANARIMemoryDeleter deleter, const void* userData, ANARIDataType type, uint64_t numItems)
    {
        helium::Array1DMemoryDescriptor md;
        md.appMemory = appMemory;
        md.deleter = deleter;
        md.deleterPtr = userData;
        md.elementType = type;
        md.numItems = numItems;

        if (anari::isObject(type))
            return (ANARIArray1D) new helium::ObjectArray(globalState(), md);
        else
            return (ANARIArray1D) new Array1D(globalState(), md);
    }

    ANARIArray2D Device::newArray2D(const void* appMemory, 
        ANARIMemoryDeleter deleter, const void* userData, 
        ANARIDataType type, uint64_t numItems1, uint64_t numItems2)
    {
        helium::Array2DMemoryDescriptor md;
        md.appMemory = appMemory;
        md.deleter = deleter;
        md.deleterPtr = userData;
        md.elementType = type;
        md.numItems1 = numItems1;
        md.numItems2 = numItems2;

        return (ANARIArray2D) new Array2D(globalState(), md);
    }

    ANARIArray3D Device::newArray3D(const void* appMemory, 
        ANARIMemoryDeleter deleter, const void* userData, 
        ANARIDataType type, uint64_t numItems1, uint64_t numItems2, uint64_t numItems3)
    {
        helium::Array3DMemoryDescriptor md;
        md.appMemory = appMemory;
        md.deleter = deleter;
        md.deleterPtr = userData;
        md.elementType = type;
        md.numItems1 = numItems1;
        md.numItems2 = numItems2;
        md.numItems3 = numItems3;

        return (ANARIArray3D) new Array3D(globalState(), md);
    }

    ANARILight Device::newLight(const char* subtype)
    {
        return (ANARILight) Light::createInstance(subtype, globalState());
    }

    ANARICamera Device::newCamera(const char* subtype)
    {
        return (ANARICamera) Camera::createInstance(subtype, globalState());
    }

    ANARIGeometry Device::newGeometry(const char* subtype)
    {
        return (ANARIGeometry) Geometry::createInstance(subtype, globalState());
    }

    ANARISpatialField Device::newSpatialField(const char* subtype)
    {
        return (ANARISpatialField) SpatialField::createInstance(subtype, globalState());
    }

    ANARISurface Device::newSurface()
    {
        return (ANARISurface) new Surface(globalState());
    }

    ANARIVolume Device::newVolume(const char* subtype)
    {
        return (ANARIVolume) Volume::createInstance(subtype, globalState());
    }

    ANARIMaterial Device::newMaterial(const char* subtype)
    {
        return (ANARIMaterial) Material::createInstance(subtype, globalState());
    }

    ANARISampler Device::newSampler(const char* subtype)
    {
        return (ANARISampler) Sampler::createInstance(subtype, globalState());
    }

    ANARIGroup Device::newGroup()
    {
        return (ANARIGroup) new Group(globalState());
    }

    ANARIInstance Device::newInstance(const char* subtype)
    {
        return (ANARIInstance) new Instance(globalState());
    }

    ANARIWorld Device::newWorld()
    {
        return (ANARIWorld) new World(globalState());
    }

    ANARIFrame Device::newFrame()
    {
        return (ANARIFrame) new Frame(globalState());
    }

    ANARIRenderer Device::newRenderer(const char* subtype)
    {
        return (ANARIRenderer) new Renderer(globalState());
    }

    const char** Device::getObjectSubtypes(ANARIDataType type)
    {
        return fgeia::query_object_types(type);
    }

    const void* Device::getObjectInfo(ANARIDataType objectType, 
        const char* objectSubtype, const char* infoName, ANARIDataType infoType)
    {
        return fgeia::query_object_info(objectType, objectSubtype, infoName, infoType);
    }

    const void* Device::getParameterInfo(ANARIDataType objectType, 
        const char* objectSubtype, const char* parameterName, 
        ANARIDataType parameterType, const char* infoName, ANARIDataType infoType)
    {
        return fgeia::query_param_info(objectType, objectSubtype, parameterName,
            parameterType, infoName, infoType);
    }

    void Device::deviceCommitParameters()
    {
        helium::BaseDevice::deviceCommitParameters();
    }

    int Device::deviceGetProperty(const char* name, ANARIDataType type, void* mem, 
        uint64_t size, uint32_t flags)
    {
        std::string_view prop = name;

        if (prop == "extension" && type == ANARI_STRING_LIST) 
        {
            helium::writeToVoidP(mem, query_extensions());
            return 1;
        } 
        else if (prop == "fge" && type == ANARI_BOOL) 
        {
            helium::writeToVoidP(mem, true);
            return 1;
        }
        return 0;
    }
}
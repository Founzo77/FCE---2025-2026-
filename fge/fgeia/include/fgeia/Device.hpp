#pragma once

#include "FgeGlobalState.hpp"

#include <helium/BaseDevice.h>

namespace fgeia
{
    class Device : public helium::BaseDevice
    {
    private:

    public:
        Device(ANARILibrary lib, const std::string& subtype = "default");
        virtual ~Device() override;

        FgeGlobalState* globalState();

        ANARIArray1D newArray1D(const void* appMemory, ANARIMemoryDeleter deleter,
            const void* userData, ANARIDataType type, uint64_t numItems) override;
        ANARIArray2D newArray2D(const void* appMemory, ANARIMemoryDeleter deleter,
            const void* userData, ANARIDataType type, uint64_t numItems1, 
            uint64_t numItems2) override;
        ANARIArray3D newArray3D(const void* appMemory, ANARIMemoryDeleter deleter,
            const void* userData, ANARIDataType type, uint64_t numItems1, 
            uint64_t numItems2, uint64_t numItems3) override;

        ANARILight newLight(const char* subtype) override;
        ANARICamera newCamera(const char* subtype) override;
        ANARIGeometry newGeometry(const char* subtype) override;
        ANARISpatialField newSpatialField(const char* subtype) override;
        ANARISurface newSurface() override;
        ANARIVolume newVolume(const char* subtype) override;
        ANARIMaterial newMaterial(const char* subtype) override;
        ANARISampler newSampler(const char* subtype) override;
        ANARIGroup newGroup() override;
        ANARIInstance newInstance(const char* subtype) override;
        ANARIWorld newWorld() override;
        ANARIFrame newFrame() override;
        ANARIRenderer newRenderer(const char* subtype) override;

        const char** getObjectSubtypes(ANARIDataType type) override;
        const void* getObjectInfo(ANARIDataType objectType,
            const char* objectSubtype, const char* infoName, ANARIDataType infoType) override;
        const void* getParameterInfo(ANARIDataType objectType, const char* objectSubtype, 
            const char* parameterName, ANARIDataType parameterType,
            const char* infoName, ANARIDataType infoType) override;

    protected:
        void deviceCommitParameters() override;
        int deviceGetProperty(const char* name, ANARIDataType type, void* mem,
            uint64_t size, uint32_t flags) override;
    };
}
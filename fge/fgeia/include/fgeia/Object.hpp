#pragma once 

#include "FgeGlobalState.hpp"
#include "utility.hpp"

#include <helium/BaseObject.h>

namespace fgeia
{
    class Object : public helium::BaseObject
    {
    public:
        Object(ANARIDataType type, FgeGlobalState* globalState);
        virtual ~Object() = default;

        virtual bool getProperty(const std::string_view& name,
            ANARIDataType type, void* ptr, uint64_t size, uint32_t flags) override;

        virtual void commitParameters() override;
        virtual void finalize() override;
        virtual bool isValid() const override;

        FgeGlobalState* globalState();
    };

    class UnknownObject : public Object
    {
    public:
        UnknownObject(ANARIDataType type, std::string_view subtype, FgeGlobalState* globalState);
        ~UnknownObject() override = default;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Object*, ANARI_OBJECT);

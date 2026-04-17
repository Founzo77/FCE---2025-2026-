#include <fgeia/Object.hpp>

namespace fgeia
{
    Object::Object(ANARIDataType type, FgeGlobalState* globalState) :
        helium::BaseObject(type, globalState)
    {

    }

    bool Object::getProperty(const std::string_view& name, 
        ANARIDataType type, void* ptr, uint64_t size, uint32_t flags)
    {
        if (name == "valid" && type == ANARI_BOOL) 
        {
            helium::writeToVoidP(ptr, isValid());
            return true;
        }
        
        return false;
    }

    void Object::commitParameters()
    {

    }

    void Object::finalize()
    {

    }

    bool Object::isValid() const
    {
        return true;
    }

    FgeGlobalState* Object::globalState()
    {
        return (FgeGlobalState*) helium::BaseObject::m_state;
    }

    UnknownObject::UnknownObject(ANARIDataType type, 
        std::string_view subtype, FgeGlobalState* globalState) :
        Object(type, globalState)
    {
        reportMessage(ANARI_SEVERITY_WARNING, 
            "fgeia object type '%s' of subtype '%s' not implemented",
            anari::toString(type), std::string(subtype).c_str());
    }

    bool UnknownObject::isValid() const
    {
        return false;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Object*);

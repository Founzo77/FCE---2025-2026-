#pragma once

#include <anari/anari.h>

#include <string>

#define FGE_ANARI_TYPEFOR_SPECIALIZATION(type, anari_type)      \
namespace anari {                                               \
    ANARI_TYPEFOR_SPECIALIZATION(type, anari_type);             \
}

#define FGE_ANARI_TYPEFOR_DEFINITION(type)      \
namespace anari {                               \
    ANARI_TYPEFOR_DEFINITION(type);             \
}

namespace fgeia
{
    template <typename SCALAR_T>
    struct AnariParameter
    {
        SCALAR_T value;
        std::string attribute;
    };
}
#pragma once

#include <string>

namespace fgewa
{
    inline bool hasSubtype(const char **subs, const char *name) noexcept
    {
        if (!subs) return false;
        for (int i=0; subs[i]; ++i) if (std::strcmp(subs[i],name)==0) return true;
        return false;
    }
}
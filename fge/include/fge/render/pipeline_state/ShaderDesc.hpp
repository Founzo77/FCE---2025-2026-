#pragma once

#include <string>

using std::string;
using std::wstring;

namespace fge
{
    enum class ShaderType
    {
        MISS,
        RAY_GEN,
        CLOSEST_HIT,
        INTERSECTION,
        ANY_HIT
    };

    struct ShaderDesc
    {
        wstring m_name;
        string m_compiledShaderPathFile;
        ShaderType m_type;

        ShaderDesc() {}

        ShaderDesc(const wstring& name, const string& compiledShaderPathFile,
            const ShaderType type) : m_name(name), m_compiledShaderPathFile(compiledShaderPathFile),
            m_type(type)
        {
            
        }

        ShaderDesc(const ShaderDesc&) = default;
        ShaderDesc(ShaderDesc&&) = default;

        ShaderDesc& operator=(const ShaderDesc&) = default;
        ShaderDesc& operator=(ShaderDesc&&) = default;
    };
}
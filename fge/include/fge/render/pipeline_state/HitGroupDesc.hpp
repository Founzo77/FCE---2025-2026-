#pragma once 

#include <string>
#include <optional>

using std::wstring;
using std::optional;

namespace fge
{
    enum class HitGroupDomain
    {
        SURFACIQUE,
        VOLUMIQUE
    };

    struct HitGroupDesc
    {
    public:
        wstring m_name;
        optional<wstring> m_closestHitShaderName;
        optional<wstring> m_intersectionShaderName;
        optional<wstring> m_anyHitShaderName;
        HitGroupDomain m_hitGroupDomain;

        HitGroupDesc() {}

        HitGroupDesc(const wstring& name, const optional<wstring>& closestHitShaderName,
            const optional<wstring>& intersectionShaderName, 
            const optional<wstring>& anyHitShaderName, const HitGroupDomain hitGroupDomain) :
            m_name(name), m_closestHitShaderName(closestHitShaderName),
            m_intersectionShaderName(intersectionShaderName), m_anyHitShaderName(anyHitShaderName),
            m_hitGroupDomain(hitGroupDomain)
        {

        }

        HitGroupDesc(const HitGroupDesc&) = default;
        HitGroupDesc(HitGroupDesc&&) = default;

        HitGroupDesc& operator=(const HitGroupDesc&) = default;
        HitGroupDesc& operator=(HitGroupDesc&&) = default;
    };
}
#include <fge/render/pipeline_state/ShaderBank.hpp>

#include <fge/resources/ShaderFactory.hpp>

#include <fge/utility.hpp>

namespace fge
{
    const vector<ShaderDesc> ShaderBank::SHADERS_DESCRIPTIONS =
    {
        { L"RayGenShader", "shaders/bin/raygen.dxil", ShaderType::RAY_GEN },
        { L"MissShader", "shaders/bin/miss.dxil", ShaderType::MISS },
        { L"PhongClosestHit", "shaders/bin/phong_closest_hit.dxil", ShaderType::CLOSEST_HIT },
        { L"OldPhongClosestHit", "shaders/bin/old_phong_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"DiffuseClosestHit", "shaders/bin/diffuse_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"NormalClosestHit", "shaders/bin/normal_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"AlbedoClosestHit", "shaders/bin/albedo_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"VolumeIntersection", "shaders/bin/volume_intersection.dxil", 
            ShaderType::INTERSECTION },
        { L"DvrClosestHit", "shaders/bin/dvr_closest_hit.dxil", ShaderType::CLOSEST_HIT },
        { L"DvrAnyHit", "shaders/bin/dvr_any_hit.dxil", ShaderType::ANY_HIT },
        { L"DvrMaxClosestHit", "shaders/bin/dvr_max_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"DvrMinClosestHit", "shaders/bin/dvr_min_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"IsosurfaceClosestHit", "shaders/bin/isosurface_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"ParticleAnyHit", "shaders/bin/particle_any_hit.dxil", ShaderType::ANY_HIT },
        { L"EmptyVolumeClosestHit", "shaders/bin/empty_volume_closest_hit.dxil", 
            ShaderType::CLOSEST_HIT },
        { L"EmptyVolumeAnyHit", "shaders/bin/empty_volume_any_hit.dxil", 
            ShaderType::ANY_HIT },
    };

    const vector<HitGroupDesc> ShaderBank::HIT_GROUP_DESCRIPTIONS =
    {
        { L"PhongHitGroup",  L"PhongClosestHit", std::nullopt, std::nullopt, 
            HitGroupDomain::SURFACIQUE },
        { L"OldPhongHitGroup",  L"OldPhongClosestHit", std::nullopt, std::nullopt, 
            HitGroupDomain::SURFACIQUE },
        { L"DiffuseHitGroup",  L"DiffuseClosestHit", std::nullopt, std::nullopt, 
            HitGroupDomain::SURFACIQUE },
        { L"NormalHitGroup",  L"NormalClosestHit", std::nullopt, std::nullopt, 
            HitGroupDomain::SURFACIQUE },
        { L"AlbedoHitGroup",  L"AlbedoClosestHit", std::nullopt, std::nullopt, 
            HitGroupDomain::SURFACIQUE },
        { L"InvisibleVolumeHitGroup", L"EmptyVolumeClosestHit", L"VolumeIntersection", 
            L"EmptyVolumeAnyHit", HitGroupDomain::VOLUMIQUE },
        { L"DvrHitGroup", L"DvrClosestHit", L"VolumeIntersection", 
            L"DvrAnyHit", HitGroupDomain::VOLUMIQUE },
        { L"DvrMaxHitGroup", L"DvrMaxClosestHit", L"VolumeIntersection", std::nullopt,
            HitGroupDomain::VOLUMIQUE },
        { L"DvrMinHitGroup", L"DvrMinClosestHit", L"VolumeIntersection", std::nullopt,
            HitGroupDomain::VOLUMIQUE },
        { L"IsosurfaceHitGroup", L"IsosurfaceClosestHit", L"VolumeIntersection", 
            std::nullopt, HitGroupDomain::VOLUMIQUE },
        { L"ParticleHitGroup", L"EmptyVolumeClosestHit", L"VolumeIntersection", 
            L"ParticleAnyHit", HitGroupDomain::VOLUMIQUE }
    };

    const vector<D3D12_STATE_SUBOBJECT>& fge::ShaderBank::getStateSubObjects() const
    {
        return m_stateSubObjects;
    }

    uint32_t ShaderBank::getHitGroupIndex(const wstring& hitGroupName)
    {
        for (uint32_t i = 0; i < HIT_GROUP_DESCRIPTIONS.size(); ++i)
        {
            if (HIT_GROUP_DESCRIPTIONS[i].m_name == hitGroupName)
            {
                return i;
            }
        }

        throwIfFailed(false, "HitGroup not found");

        return 0;
    }

    void* ShaderBank::getMissShaderIdentifier()
    {
        return m_missShaderIdentifier;
    }

    void* ShaderBank::getRayGenShaderIdentifier()
    {
        return m_rayGenShaderIdentifier;
    }

    void* ShaderBank::getHitGroupIdentifier(const uint32_t hitGroupIndex)
    {
        throwIfFailed(hitGroupIndex < m_hitGroupIdentifiers.size(), "Invalid HitGroup index");

        return m_hitGroupIdentifiers[hitGroupIndex];
    }

    const vector<wstring>& ShaderBank::getSurfaciqueHitGroupNames() const
    {
        return m_surfaciqueHitGroupNames;
    }

    const vector<wstring>& ShaderBank::getVolumiqueHitGroupNames() const
    {
        return m_volumiqueHitGroupNames;
    }

    void ShaderBank::initialize()
    {
        m_exportDescriptions.clear();
        m_libraryDescriptions.clear();
        m_compiledShaders.clear();
        m_hitGroupDescriptions.clear();
        m_stateSubObjects.clear();

        m_surfaciqueHitGroupNames.clear();
        m_volumiqueHitGroupNames.clear();

        // Reserve pour éviter les reallocations (important pour les pointeurs)
        m_exportDescriptions.reserve(SHADERS_DESCRIPTIONS.size());
        m_libraryDescriptions.reserve(SHADERS_DESCRIPTIONS.size());
        m_compiledShaders.reserve(SHADERS_DESCRIPTIONS.size());

        // DXIL Libraries
        for (const ShaderDesc& shader : SHADERS_DESCRIPTIONS)
        {
            m_compiledShaders.emplace_back(
                ShaderFactory::loadShader(shader.m_compiledShaderPathFile));

            D3D12_EXPORT_DESC exportDesc = {};
            exportDesc.Name = shader.m_name.c_str();
            exportDesc.ExportToRename = nullptr;
            exportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

            m_exportDescriptions.push_back(exportDesc);

            D3D12_DXIL_LIBRARY_DESC libDesc = {};
            libDesc.DXILLibrary.pShaderBytecode = m_compiledShaders.back().data();
            libDesc.DXILLibrary.BytecodeLength = m_compiledShaders.back().size();
            libDesc.NumExports = 1;
            libDesc.pExports = &m_exportDescriptions.back();

            m_libraryDescriptions.push_back(libDesc);

            // Subobject
            D3D12_STATE_SUBOBJECT subObject = {};
            subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
            subObject.pDesc = &m_libraryDescriptions.back();

            m_stateSubObjects.push_back(subObject);
        }

        // Hit Groups
        m_hitGroupDescriptions.reserve(HIT_GROUP_DESCRIPTIONS.size());
        m_surfaciqueHitGroupNames.reserve(HIT_GROUP_DESCRIPTIONS.size());
        m_volumiqueHitGroupNames.reserve(HIT_GROUP_DESCRIPTIONS.size());

        for (const HitGroupDesc& hg : HIT_GROUP_DESCRIPTIONS)
        {
            D3D12_HIT_GROUP_DESC hitGroup = {};
            hitGroup.HitGroupExport = hg.m_name.c_str();
            if (hg.m_closestHitShaderName.has_value())
            {
                hitGroup.ClosestHitShaderImport = hg.m_closestHitShaderName->c_str();
            }
            else
            {
                hitGroup.ClosestHitShaderImport = nullptr;
            }

            if (hg.m_intersectionShaderName.has_value())
            {
                hitGroup.IntersectionShaderImport = hg.m_intersectionShaderName->c_str();
                hitGroup.Type = D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE;
            }
            else
            {
                hitGroup.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
            }

            if (hg.m_anyHitShaderName.has_value())
            {
                hitGroup.AnyHitShaderImport = hg.m_anyHitShaderName->c_str();
            }

            m_hitGroupDescriptions.push_back(hitGroup);

            D3D12_STATE_SUBOBJECT hitGroupSubObject = {};
            hitGroupSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
            hitGroupSubObject.pDesc = &m_hitGroupDescriptions.back();

            m_stateSubObjects.push_back(hitGroupSubObject);

            if(hg.m_hitGroupDomain == HitGroupDomain::SURFACIQUE)
            {
                m_surfaciqueHitGroupNames.push_back(hg.m_name);
            }
            else if(hg.m_hitGroupDomain == HitGroupDomain::VOLUMIQUE)
            {
                m_volumiqueHitGroupNames.push_back(hg.m_name);
            }
        }
    }

    void ShaderBank::resolveShaderIdentifiers(ComPtr<ID3D12StateObjectProperties> pipelineProperties)
    {
        m_missShaderIdentifier = nullptr;
        m_rayGenShaderIdentifier = nullptr;

        // RayGen
        for (const ShaderDesc& shader : SHADERS_DESCRIPTIONS)
        {
            if (shader.m_type == ShaderType::RAY_GEN)
            {
                m_rayGenShaderIdentifier =
                    pipelineProperties->GetShaderIdentifier(shader.m_name.c_str());

                throwIfFailed(m_rayGenShaderIdentifier != nullptr,
                    "Failed to get RayGen shader identifier");
                break;
            }
        }

        // Miss
        for (const ShaderDesc& shader : SHADERS_DESCRIPTIONS)
        {
            if (shader.m_type == ShaderType::MISS)
            {
                m_missShaderIdentifier =
                    pipelineProperties->GetShaderIdentifier(shader.m_name.c_str());

                throwIfFailed(m_missShaderIdentifier != nullptr,
                    "Failed to get Miss shader identifier");
                break;
            }
        }

        // HitGroups
        m_hitGroupIdentifiers.clear();
        m_hitGroupIdentifiers.reserve(HIT_GROUP_DESCRIPTIONS.size());

        for (const HitGroupDesc& hg : HIT_GROUP_DESCRIPTIONS)
        {
            void* id = pipelineProperties->GetShaderIdentifier(hg.m_name.c_str());

            throwIfFailed(id != nullptr,
                "Failed to get HitGroup shader identifier");

            m_hitGroupIdentifiers.push_back(id);
        }
    }
}
#pragma once

#include "ShaderDesc.hpp"
#include "HitGroupDesc.hpp"

#include <wrl/client.h>
#include <d3dx12.h>

#include <vector>

using namespace Microsoft::WRL;
using std::vector;
using std::wstring;

namespace fge
{
    class ShaderBank
    {
    public:
        static const vector<ShaderDesc> SHADERS_DESCRIPTIONS;
        static const vector<HitGroupDesc> HIT_GROUP_DESCRIPTIONS;

    private:
        vector<D3D12_EXPORT_DESC> m_exportDescriptions;
        vector<D3D12_DXIL_LIBRARY_DESC> m_libraryDescriptions;
        vector<vector<char>> m_compiledShaders;
        vector<D3D12_HIT_GROUP_DESC> m_hitGroupDescriptions;
        vector<D3D12_STATE_SUBOBJECT> m_stateSubObjects;
        vector<wstring> m_surfaciqueHitGroupNames;
        vector<wstring> m_volumiqueHitGroupNames;
        vector<void*> m_hitGroupIdentifiers;
        void* m_missShaderIdentifier;
        void* m_rayGenShaderIdentifier;

    public:

        const vector<D3D12_STATE_SUBOBJECT>& getStateSubObjects() const;

        static uint32_t getHitGroupIndex(const wstring& hitGroupName);
        void* getMissShaderIdentifier();
        void* getRayGenShaderIdentifier();
        void* getHitGroupIdentifier(const uint32_t hitGroupIndex);

        const vector<wstring>& getSurfaciqueHitGroupNames() const;
        const vector<wstring>& getVolumiqueHitGroupNames() const;

        void initialize();
        void resolveShaderIdentifiers(ComPtr<ID3D12StateObjectProperties> pipelineProperties);
    };
}
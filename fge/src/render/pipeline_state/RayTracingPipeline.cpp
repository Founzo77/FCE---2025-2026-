#include <fge/render/pipeline_state/RayTracingPipeline.hpp>
#include <fge/render/Scene.hpp>

#include <fge/render/data/SubMeshData.hpp>

#include <fge/resources/DescriptorHeapFactory.hpp>
#include <fge/resources/ShaderFactory.hpp>
#include <fge/utility.hpp>

#include <d3dx12.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    RayTracingPipeline::~RayTracingPipeline()
    {
        reset();
    }

    void RayTracingPipeline::initialize(ComPtr<ID3D12Device5> device, 
        shared_ptr<SceneMemoryManager> sceneMemory, uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        m_sceneMemory = sceneMemory;
        m_device = device;

        // TO_DO voir si vaut mieux pas deplacer au dessus dans la stack
        m_sceneMemory->initializeOutputTexture(m_device, width, height);

        m_shaderBank.initialize();
        
        initializecbvSrvUavResources();
        initializeRootSignature();
        initializePipelineState();
        m_shaderBank.resolveShaderIdentifiers(m_pipelineProperties);
        initializeSBT();
    }

    void RayTracingPipeline::reset()
    {
        m_width = 0;
        m_height = 0;
        m_handleView.reset();
        m_handleHostView.reset();

        m_globalRootSignature.Reset();
        m_localRootSurfaceSignature.Reset();
        m_localRootVolumeSignature.Reset();
        m_pipelineState.Reset();
        m_pipelineProperties.Reset();

        m_device.Reset();

        m_sceneMemory.reset();
    }

    CbvSrvUavView& RayTracingPipeline::getHandleView()
    {
        return m_handleView;
    }

    CbvSrvUavView& RayTracingPipeline::getHandleHostView()
    {
        return m_handleHostView;
    }

    uint32_t RayTracingPipeline::getWidth() const noexcept
    {
        return m_width;
    }

    uint32_t RayTracingPipeline::getHeight() const noexcept
    {
        return m_height;
    }

    void RayTracingPipeline::resize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;

        m_sceneMemory->resizeOutputTexture(m_device, width, height);
        m_handleView.resize(m_device, *m_sceneMemory);
        m_handleHostView.resize(m_device, *m_sceneMemory);
        // TO_DO re appeler initializecbvSrvUavResources
    }

    void RayTracingPipeline::bindDescriptors(ComPtr<ID3D12GraphicsCommandList4> directCommandList,
        const uint32_t cumputeFrameIndex)
    {
        ID3D12DescriptorHeap* heaps[] = { m_handleView.getDescriptorHeap() };

        directCommandList->SetComputeRootSignature(m_globalRootSignature.Get());
        directCommandList->SetDescriptorHeaps(1, heaps);

        // TO_DO Changer les valeurs brutes des index par des constexpr
        directCommandList->SetComputeRootConstantBufferView(0,
            m_sceneMemory->getConstantUploadBufferAddress(cumputeFrameIndex));
        directCommandList->SetComputeRootDescriptorTable(1, 
            m_handleView.getOutputTextureUavHandle());
        directCommandList->SetComputeRootDescriptorTable(2, 
            m_handleView.getAccumulatedInputTextureUavHandle());
        directCommandList->SetComputeRootDescriptorTable(3, 
            m_handleView.getAccumulatedOutputTextureUavHandle());
        directCommandList->SetComputeRootDescriptorTable(4, m_handleView.getTlasSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(5, m_handleView.getMaterialSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(6, 
            m_handleView.getIndirectionMaterialTableSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(7, m_handleView.getLightSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(8, m_handleView.getBaseTextureSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(9, m_handleView.getBaseTexture3DSrvHandle());
    }

    void RayTracingPipeline::dispatchRays(ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
        dispatchDesc.Width = m_width;
        dispatchDesc.Height = m_height;
        dispatchDesc.Depth = 1;

        dispatchDesc.RayGenerationShaderRecord = m_sbtTable.getRaygenerationShaderRecord();
        dispatchDesc.MissShaderTable = m_sbtTable.getMissShaderTable();
        dispatchDesc.HitGroupTable = m_sbtTable.getHitGroupTable();

        directCommandList->SetPipelineState1(m_pipelineState.Get());
        directCommandList->DispatchRays(&dispatchDesc);
    }

    void RayTracingPipeline::initializecbvSrvUavResources()
    {
        m_handleView.initialize(m_device, *m_sceneMemory, 
            D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        m_handleHostView.initialize(m_device, *m_sceneMemory, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    }

    void RayTracingPipeline::initializeRootSignature()
    {
        CD3DX12_DESCRIPTOR_RANGE1 outputTextureUavRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 
            SHADER_REGISTER_OUTPUT, SHADER_SPACE_SCENE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 accumulatedInputTextureUavRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, SHADER_REGISTER_ACCUMULATED_INPUT, 
            SHADER_SPACE_SCENE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 accumulatedOutputTextureUavRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, SHADER_REGISTER_ACCUMULATED_OUTPUT, 
            SHADER_SPACE_SCENE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 tlasSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_SCENE_TLAS, SHADER_SPACE_SCENE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 lightSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_LIGHTS, SHADER_SPACE_SCENE_DATA);

        CD3DX12_DESCRIPTOR_RANGE1 materialSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_MATERIALS, SHADER_SPACE_SURFACIQUE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 indirectionTableMaterialSrvRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_INDIRECTION_MATERIAL_TABLE, SHADER_SPACE_SURFACIQUE_DATA);

        // TO_DO : retirer le D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE
        CD3DX12_DESCRIPTOR_RANGE1 texturesSrvRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, m_sceneMemory->getNbMaxTextures(), 
            SHADER_REGISTER_TEXTURES, SHADER_SPACE_SURFACIQUE_DATA,
            D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

        CD3DX12_DESCRIPTOR_RANGE1 textures3DSrvRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, m_sceneMemory->getNbMaxTextures3D(), 
            SHADER_REGISTER_TEXTURES_3D, SHADER_SPACE_VOLUMIQUE_DATA,
            D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
        
        // TO_DO Changer les valeurs brutes des index par des constexpr
        CD3DX12_ROOT_PARAMETER1 globalRootParams[10];
        globalRootParams[0].InitAsConstantBufferView(
            SHADER_REGISTER_CONSTANT_INFORMATION, SHADER_SPACE_SCENE_DATA);
        globalRootParams[1].InitAsDescriptorTable(1, &outputTextureUavRange);
        globalRootParams[2].InitAsDescriptorTable(1, &accumulatedInputTextureUavRange);
        globalRootParams[3].InitAsDescriptorTable(1, &accumulatedOutputTextureUavRange);
        globalRootParams[4].InitAsDescriptorTable(1, &tlasSrvRange);
        globalRootParams[5].InitAsDescriptorTable(1, &materialSrvRange);
        globalRootParams[6].InitAsDescriptorTable(1, &indirectionTableMaterialSrvRange);
        globalRootParams[7].InitAsDescriptorTable(1, &lightSrvRange);
        globalRootParams[8].InitAsDescriptorTable(1, &texturesSrvRange);
        globalRootParams[9].InitAsDescriptorTable(1, &textures3DSrvRange);

        CD3DX12_STATIC_SAMPLER_DESC samplerDesc(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC globalRootDesc;
        globalRootDesc.Init_1_1(_countof(globalRootParams), globalRootParams, 1, 
            &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_NONE);

        ComPtr<ID3DBlob> sigBlob, errorBlob;
        throwIfFailed(D3DX12SerializeVersionedRootSignature(&globalRootDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1_1, &sigBlob, &errorBlob),
            "Failed to serialize global Root Signature");

        throwIfFailed(m_device->CreateRootSignature(0, sigBlob->GetBufferPointer(),
            sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_globalRootSignature)),
            "Failed to create global Root Signature");
        d12SetDebugName(m_globalRootSignature, L"Global Root Signature");

        CD3DX12_DESCRIPTOR_RANGE1 verticesSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_VERTICES, SHADER_SPACE_SURFACIQUE_DATA);
        CD3DX12_DESCRIPTOR_RANGE1 indicesSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_INDICES, SHADER_SPACE_SURFACIQUE_DATA);
        
        // UINT_MAX = "unbounded array"
        // TO_DO Verifier la pertinence du UINT_MAX
        
        CD3DX12_ROOT_PARAMETER1 localRootParams[3];
        localRootParams[0].InitAsDescriptorTable(1, &verticesSrvRange);
        localRootParams[1].InitAsDescriptorTable(1, &indicesSrvRange);
        localRootParams[2].InitAsConstants(1, 
            SHADER_REGISTER_HIT_LOCAL, SHADER_SPACE_SURFACIQUE_DATA);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC localRootDesc;
        localRootDesc.Init_1_1(_countof(localRootParams), localRootParams, 0, 
            nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

        throwIfFailed(D3DX12SerializeVersionedRootSignature(&localRootDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1_1, &sigBlob, &errorBlob),
            "Failed to serialize local Root Signature");

        throwIfFailed(m_device->CreateRootSignature(0, sigBlob->GetBufferPointer(),
            sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_localRootSurfaceSignature)),
            "Failed to create local Root Signature");
        d12SetDebugName(m_localRootSurfaceSignature, L"Local Root Surface Signature");

        CD3DX12_ROOT_PARAMETER1 localVolumeRootParams[1];
        localVolumeRootParams[0].InitAsConstants(12, SHADER_REGISTER_HIT_LOCAL,
            SHADER_SPACE_VOLUMIQUE_DATA);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC localVolumeRootDesc;
        localVolumeRootDesc.Init_1_1(_countof(localVolumeRootParams), localVolumeRootParams, 0, 
            nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

        throwIfFailed(D3DX12SerializeVersionedRootSignature(&localVolumeRootDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1_1, &sigBlob, &errorBlob),
            "Failed to serialize local Root Signature");

        throwIfFailed(m_device->CreateRootSignature(0, sigBlob->GetBufferPointer(),
            sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_localRootVolumeSignature)),
            "Failed to create local Root Signature");
        d12SetDebugName(m_localRootVolumeSignature, L"Local Root Volume Signature");
    }

    void RayTracingPipeline::initializePipelineState()
    {
        // TO_DO Ajouter verification sur la taille du nombre des elements de shaderBank

        vector<D3D12_STATE_SUBOBJECT> subObjects;
        // TO_DO reserve le bon nombre pour pas avoir de probleme
        subObjects.reserve(150);

        for(const D3D12_STATE_SUBOBJECT& subObject : m_shaderBank.getStateSubObjects())
        {
            subObjects.push_back(subObject);
        }
        
        // GRS
        ID3D12RootSignature* rootSig = m_globalRootSignature.Get();
        D3D12_STATE_SUBOBJECT globalRootSubObject;
        globalRootSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        globalRootSubObject.pDesc = &rootSig;
        subObjects.push_back(globalRootSubObject);

        // LRS
        ID3D12RootSignature* localSig = m_localRootSurfaceSignature.Get();
        D3D12_STATE_SUBOBJECT localSurfaceRootSubObject;
        localSurfaceRootSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        localSurfaceRootSubObject.pDesc = &localSig;
        size_t idxLocalSurface = subObjects.size();
        subObjects.push_back(localSurfaceRootSubObject);

        ID3D12RootSignature* localVolumeSig = m_localRootVolumeSignature.Get();
        D3D12_STATE_SUBOBJECT localRootVolumeSubObject = {};
        localRootVolumeSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        localRootVolumeSubObject.pDesc = &localVolumeSig;
        size_t idxLocalVolume = subObjects.size();
        subObjects.push_back(localRootVolumeSubObject);

        // LRS Surface HitGroups surfaciques
        const vector<wstring>& surfaciqueHitGroups = m_shaderBank.getSurfaciqueHitGroupNames();

        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION lrsSurfaceAssociation = {};
        lrsSurfaceAssociation.NumExports = static_cast<UINT>(surfaciqueHitGroups.size());

        // IMPORTANT : tableau de LPCWSTR stable
        std::vector<LPCWSTR> surfaciqueExports;
        surfaciqueExports.reserve(surfaciqueHitGroups.size());

        for (const std::wstring& name : surfaciqueHitGroups)
        {
            surfaciqueExports.push_back(name.c_str());
        }

        lrsSurfaceAssociation.pExports = surfaciqueExports.data();
        lrsSurfaceAssociation.pSubobjectToAssociate = &subObjects[idxLocalSurface];

        D3D12_STATE_SUBOBJECT lrsSurfaceSubObject = {};
        lrsSurfaceSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        lrsSurfaceSubObject.pDesc = &lrsSurfaceAssociation;

        subObjects.push_back(lrsSurfaceSubObject);

        // LRS Volume HitGroups volumique
        const vector<wstring>& volumiqueHitGroups = m_shaderBank.getVolumiqueHitGroupNames();

        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION lrsVolumeAssociation = {};
        lrsVolumeAssociation.NumExports =
            static_cast<UINT>(volumiqueHitGroups.size());

        std::vector<LPCWSTR> volumiqueExports;
        volumiqueExports.reserve(volumiqueHitGroups.size());

        for (const std::wstring& name : volumiqueHitGroups)
        {
            volumiqueExports.push_back(name.c_str());
        }

        lrsVolumeAssociation.pExports = volumiqueExports.data();
        lrsVolumeAssociation.pSubobjectToAssociate = &subObjects[idxLocalVolume];

        D3D12_STATE_SUBOBJECT lrsVolumeSubObject = {};
        lrsVolumeSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        lrsVolumeSubObject.pDesc = &lrsVolumeAssociation;

        subObjects.push_back(lrsVolumeSubObject);


        D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
        shaderConfig.MaxPayloadSizeInBytes = sizeof(uint32_t) * 12;
        shaderConfig.MaxAttributeSizeInBytes = sizeof(XMFLOAT2);
        // TO_DO Pour mieux : 
        // shaderConfig.MaxAttributeSizeInBytes = max(sizeof(XMFLOAT2), sizeof(VolumeIntersectionAttributes));
        D3D12_STATE_SUBOBJECT shaderConfigSubObject = {};
        shaderConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        shaderConfigSubObject.pDesc = &shaderConfig;
        subObjects.push_back(shaderConfigSubObject);

        D3D12_RAYTRACING_PIPELINE_CONFIG pipelineConfig = {};
        // TO_DO Mettre ca en constante et mieux le gerer
        pipelineConfig.MaxTraceRecursionDepth = 12;
        D3D12_STATE_SUBOBJECT pipelineConfigSubObject = {};
        pipelineConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        pipelineConfigSubObject.pDesc = &pipelineConfig;
        subObjects.push_back(pipelineConfigSubObject);


        D3D12_STATE_OBJECT_DESC stateDesc = {};
        stateDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
        stateDesc.NumSubobjects = (uint32_t) subObjects.size();
        stateDesc.pSubobjects = subObjects.data();

        throwIfFailed(m_device->CreateStateObject(&stateDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Failed to create ray tracing pipeline of ID3D12StateObject");
        m_pipelineState->QueryInterface(IID_PPV_ARGS(&m_pipelineProperties));
        d12SetDebugName(m_pipelineState, L"Pipeline State Object");
    }

    void RayTracingPipeline::initializeSBT()
    {
        m_sbtTable.initialize(m_device, m_shaderBank, m_handleView, *m_sceneMemory);
    }
}
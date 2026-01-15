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
    void RayTracingPipeline::initialize(ComPtr<ID3D12Device5> device, 
        shared_ptr<SceneMemoryManager> sceneMemory, 
        uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
        m_sceneMemory = sceneMemory;

        // TO_DO voir si vaut mieux pas deplacer au dessus dans la stack
        m_sceneMemory->initializeOutputTexture(device, width, height);
        
        initializecbvSrvUavResources(device);
        initializeRootSignature(device);
        initializePipelineState(device);
        initializeSBT(device);
    }

    void RayTracingPipeline::resize(ComPtr<ID3D12Device5> device, 
        uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;

        m_sceneMemory->resizeOutputTexture(device, width, height);
        m_handleView.resize(device, *m_sceneMemory);
        // TO_DO re appeler initializecbvSrvUavResources
    }

    void RayTracingPipeline::dispatchRays(ComPtr<ID3D12Device5> device, 
        ComPtr<ID3D12GraphicsCommandList4> directCommandList)
    {
        D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
        dispatchDesc.Width = m_width;
        dispatchDesc.Height = m_height;
        dispatchDesc.Depth = 1;

        dispatchDesc.RayGenerationShaderRecord = m_sbtTable.getRaygenerationShaderRecord();
        dispatchDesc.MissShaderTable = m_sbtTable.getMissShaderTable();
        dispatchDesc.HitGroupTable = m_sbtTable.getHitGroupTable();
        
        ID3D12DescriptorHeap* heaps[] = { m_handleView.getDescriptorHeap() };

        directCommandList->SetComputeRootSignature(m_globalRootSignature.Get());
        directCommandList->SetDescriptorHeaps(1, heaps);

        directCommandList->SetComputeRootConstantBufferView(0,
            m_sceneMemory->getConstantUploadBufferAddress());
        directCommandList->SetComputeRootDescriptorTable(1, m_handleView.getOutuputTextureSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(2, m_handleView.getTlasSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(3, m_handleView.getMaterialSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(4, 
            m_handleView.getIndirectionMaterialTableSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(5, m_handleView.getLightSrvHandle());
        directCommandList->SetComputeRootDescriptorTable(6, m_handleView.getBaseTextureSrvHandle());

        directCommandList->SetPipelineState1(m_pipelineState.Get());
        directCommandList->DispatchRays(&dispatchDesc);
    }

    void RayTracingPipeline::initializecbvSrvUavResources(ComPtr<ID3D12Device5> device)
    {
        m_handleView.initialize(device, *m_sceneMemory);
    }

    void RayTracingPipeline::initializeRootSignature(ComPtr<ID3D12Device5> device)
    {
        CD3DX12_DESCRIPTOR_RANGE1 outputTextureUavRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 
            SHADER_REGISTER_OUTPUT);
        CD3DX12_DESCRIPTOR_RANGE1 tlasSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_SCENE);
        CD3DX12_DESCRIPTOR_RANGE1 materialSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_MATERIALS);
        CD3DX12_DESCRIPTOR_RANGE1 indirectionTableMaterialSrvRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SHADER_REGISTER_INDIRECTION_MATERIAL_TABLE);
        CD3DX12_DESCRIPTOR_RANGE1 lightSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_LIGHTS);

        // TO_DO : retirer le D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE
        CD3DX12_DESCRIPTOR_RANGE1 texturesSrvRange(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, m_sceneMemory->getNbMaxTextures(), 
            SHADER_REGISTER_TEXTURES, 0,
            D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

        CD3DX12_ROOT_PARAMETER1 globalRootParams[7];
        globalRootParams[0].InitAsConstantBufferView(SHADER_REGISTER_CONSTANT_INFORMATION);
        globalRootParams[1].InitAsDescriptorTable(1, &outputTextureUavRange);
        globalRootParams[2].InitAsDescriptorTable(1, &tlasSrvRange);
        globalRootParams[3].InitAsDescriptorTable(1, &materialSrvRange);
        globalRootParams[4].InitAsDescriptorTable(1, &indirectionTableMaterialSrvRange);
        globalRootParams[5].InitAsDescriptorTable(1, &lightSrvRange);
        globalRootParams[6].InitAsDescriptorTable(1, &texturesSrvRange);

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

        throwIfFailed(device->CreateRootSignature(0, sigBlob->GetBufferPointer(),
            sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_globalRootSignature)),
            "Failed to create global Root Signature");

        CD3DX12_DESCRIPTOR_RANGE1 verticesSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_VERTICES);
        CD3DX12_DESCRIPTOR_RANGE1 indicesSrvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 
            SHADER_REGISTER_INDICES);
        
        // UINT_MAX = "unbounded array"
        // TO_DO Verifier la pertinence du UINT_MAX
        
        CD3DX12_ROOT_PARAMETER1 localRootParams[3];
        localRootParams[0].InitAsDescriptorTable(1, &verticesSrvRange);
        localRootParams[1].InitAsDescriptorTable(1, &indicesSrvRange);
        localRootParams[2].InitAsConstants(1, SHADER_REGISTER_HIT_LOCAL);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC localRootDesc;
        localRootDesc.Init_1_1(_countof(localRootParams), localRootParams, 0, 
            nullptr, D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

        throwIfFailed(D3DX12SerializeVersionedRootSignature(&localRootDesc, 
            D3D_ROOT_SIGNATURE_VERSION_1_1, &sigBlob, &errorBlob),
            "Failed to serialize local Root Signature");

        throwIfFailed(device->CreateRootSignature(0, sigBlob->GetBufferPointer(),
            sigBlob->GetBufferSize(), IID_PPV_ARGS(&m_localRootSignature)),
            "Failed to create local Root Signature");
    }

    void RayTracingPipeline::initializePipelineState(ComPtr<ID3D12Device5> device)
    {
        vector<D3D12_STATE_SUBOBJECT> subObjects;
        subObjects.reserve(20);

        vector<char> rayGenDxil = ShaderFactory::loadShader({ "shaders/bin/raygen.dxil" });
        vector<char> missDxil = ShaderFactory::loadShader({ "shaders/bin/miss.dxil" });
        vector<char> closestHitDxil = ShaderFactory::loadShader(
            { "shaders/bin/closesthit.dxil" });
        
        // == RayGen DXIL Library ==
        D3D12_EXPORT_DESC rayGenExport = {};
        rayGenExport.Name = L"RayGenShader"; // Nom EXACT de ton entry point HLSL
        rayGenExport.ExportToRename = nullptr;
        rayGenExport.Flags = D3D12_EXPORT_FLAG_NONE;

        D3D12_DXIL_LIBRARY_DESC rayGenDxilDesc = {};
        rayGenDxilDesc.DXILLibrary.pShaderBytecode = rayGenDxil.data();
        rayGenDxilDesc.DXILLibrary.BytecodeLength = rayGenDxil.size();
        rayGenDxilDesc.NumExports = 1;
        rayGenDxilDesc.pExports = &rayGenExport;

        D3D12_STATE_SUBOBJECT rayGenSubObject = {};
        rayGenSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        rayGenSubObject.pDesc = &rayGenDxilDesc;

        subObjects.push_back(rayGenSubObject);

        // == Miss DXIL Library ==
        D3D12_EXPORT_DESC missExport = {};
        missExport.Name = L"MissShader";
        missExport.ExportToRename = nullptr;
        missExport.Flags = D3D12_EXPORT_FLAG_NONE;

        D3D12_DXIL_LIBRARY_DESC missDxilDesc = {};
        missDxilDesc.DXILLibrary.pShaderBytecode = missDxil.data();
        missDxilDesc.DXILLibrary.BytecodeLength = missDxil.size();
        missDxilDesc.NumExports = 1;
        missDxilDesc.pExports = &missExport;

        D3D12_STATE_SUBOBJECT missSubObject = {};
        missSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        missSubObject.pDesc = &missDxilDesc;

        subObjects.push_back(missSubObject);

        // == ClosestHit DXIL Library ==
        D3D12_EXPORT_DESC closestHitExport = {};
        closestHitExport.Name = L"ClosestHitShader";
        closestHitExport.ExportToRename = nullptr;
        closestHitExport.Flags = D3D12_EXPORT_FLAG_NONE;

        D3D12_DXIL_LIBRARY_DESC closestHitDxilDesc = {};
        closestHitDxilDesc.DXILLibrary.pShaderBytecode = closestHitDxil.data();
        closestHitDxilDesc.DXILLibrary.BytecodeLength = closestHitDxil.size();
        closestHitDxilDesc.NumExports = 1;
        closestHitDxilDesc.pExports = &closestHitExport;

        D3D12_STATE_SUBOBJECT hitSubObject = {};
        hitSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        hitSubObject.pDesc = &closestHitDxilDesc;

        subObjects.push_back(hitSubObject);

        D3D12_HIT_GROUP_DESC triangleHitGroupDesc = {};
        triangleHitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
        triangleHitGroupDesc.HitGroupExport = L"BasicHitGroup";
        triangleHitGroupDesc.ClosestHitShaderImport = L"ClosestHitShader";
        D3D12_STATE_SUBOBJECT triangleSubObject = {};
        triangleSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        triangleSubObject.pDesc = &triangleHitGroupDesc;
        subObjects.push_back(triangleSubObject);
        
        // GRS
        ID3D12RootSignature* rootSig = m_globalRootSignature.Get();
        D3D12_STATE_SUBOBJECT globalRootSubObject;
        globalRootSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        globalRootSubObject.pDesc = &rootSig;
        subObjects.push_back(globalRootSubObject);

        // LRS
        ID3D12RootSignature* localSig = m_localRootSignature.Get();
        D3D12_STATE_SUBOBJECT localRootSubObject;
        localRootSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        localRootSubObject.pDesc = &localSig;
        subObjects.push_back(localRootSubObject);

        D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION lrsAssociation = {};
        lrsAssociation.NumExports = 1;
        LPCWSTR exportsShaderName[] = { L"BasicHitGroup" };
        lrsAssociation.pExports = exportsShaderName;
        lrsAssociation.pSubobjectToAssociate = &subObjects.back();
        D3D12_STATE_SUBOBJECT lrsAssociationSubObject = {};
        lrsAssociationSubObject.Type = 
            D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        lrsAssociationSubObject.pDesc = &lrsAssociation;
        subObjects.push_back(lrsAssociationSubObject);

        D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
        shaderConfig.MaxPayloadSizeInBytes = sizeof(uint32_t) * 12;
        shaderConfig.MaxAttributeSizeInBytes = sizeof(XMFLOAT2);
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

        throwIfFailed(device->CreateStateObject(&stateDesc, IID_PPV_ARGS(&m_pipelineState)),
            "Failed to create ray tracing pipeline of ID3D12StateObject");
        m_pipelineState->QueryInterface(IID_PPV_ARGS(&m_pipelineProperties));
    }

    void RayTracingPipeline::initializeSBT(ComPtr<ID3D12Device5> device)
    {
        m_sbtTable.initialize(device, 
            m_pipelineProperties->GetShaderIdentifier(L"RayGenShader"),
            m_pipelineProperties->GetShaderIdentifier(L"MissShader"),
            m_pipelineProperties->GetShaderIdentifier(L"BasicHitGroup"),
            m_handleView, *m_sceneMemory);
    }
}
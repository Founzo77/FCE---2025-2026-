#include "../common/global_signature.hlsli"
#include "../common/surfacique_local_signature.hlsli"
#include "../common/maths.hlsli"

inline void resolveAlbedoIntegrator(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    uint idTriangle = PrimitiveIndex();
    uint3 triangleIndices = g_indices[idTriangle];

    Vertex v0 = g_vertices[triangleIndices.x];
    Vertex v1 = g_vertices[triangleIndices.y];
    Vertex v2 = g_vertices[triangleIndices.z];

    SubMeshData subMeshData = g_subMeshData;
    Material material = g_materials[g_logicalMaterialIndexToPhysical[subMeshData.m_materialIndex]];

    uint textureIndex = material.m_albedoTextureIndex;
    float2 uv = computeUv(v0, v1, v2, attr);

    float3 albedo = float3(0, 0, 0);

    if (textureIndex != UINT32_MAX) // UINT_32_MAX
    {
        uint idx = NonUniformResourceIndex(textureIndex);
        albedo = g_textures[idx].SampleLevel(g_sampler, uv, 0.0f).rgb;
    }

    float3 finalColor = albedo;

    float3 currentColor = mainRayPayloadGetColor(payload);
    finalColor = finalColor + currentColor;

    mainRayPayloadSetColor(payload, finalColor);
}

inline void handleMainRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    resolveAlbedoIntegrator(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    SubMeshData subMeshData = g_subMeshData;
    Material material = g_materials[g_logicalMaterialIndexToPhysical[subMeshData.m_materialIndex]];

    float3 transmittanceWeight = material.m_transmittance;
    uint nbBounds = shadowRayPayloadGetNbBounds(payload);

    if(computeRadiance(transmittanceWeight) >= MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND
        && nbBounds < MAX_RAY_NB_BOUNDS)
    {
        float3 rayDir = WorldRayDirection();
        float tHit = RayTCurrent();
        float tMax = shadowRayPayloadGetMaxHitDistance(payload);

        float3 newOrigin = WorldRayOrigin() + rayDir * (tHit + 1e-4);

        RayDesc rayDesc;
        rayDesc.Origin = newOrigin;
        rayDesc.Direction = rayDir;
        rayDesc.TMin = 0.001f;
        rayDesc.TMax = max(0.0f, tMax - tHit - 1e-4);

        RayPayload newPayload;
        newPayload.m_type = RAY_TYPE_SHADOW;

        float3 newWeight = shadowRayPayloadGetWeight(payload) * transmittanceWeight;
        shadowRayPayloadSetMaxHitDistance(newPayload, rayDesc.TMax);
        shadowRayPayloadSetWeight(newPayload, newWeight);
        shadowRayPayloadSetNbBounds(newPayload, nbBounds + 1);

        TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, rayDesc, newPayload);

        shadowRayPayloadSetWeight(payload, shadowRayPayloadGetWeight(newPayload));
        shadowRayPayloadSetInstanceId(payload, shadowRayPayloadGetInstanceId(newPayload));
    }
    else
    {
        shadowRayPayloadSetInstanceId(payload, InstanceID());
    }
}

inline void handleNearestRay(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
}

[shader("closesthit")]
void AlbedoClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_MAIN:
            handleMainRay(payload, attr);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowRay(payload, attr);
            break;
        case RAY_TYPE_NEAREST:
            handleNearestRay(payload, attr);
            break;
        default:
            mainRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
}


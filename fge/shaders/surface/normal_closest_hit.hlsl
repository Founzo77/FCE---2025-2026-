#include "../common/global_signature.hlsli"
#include "../common/surfacique_local_signature.hlsli"
#include "../common/maths.hlsli"

inline void resolveNormalIntegrator(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    uint idTriangle = PrimitiveIndex();
    uint3 triangleIndices = g_indices[idTriangle];

    Vertex v0 = g_vertices[triangleIndices.x];
    Vertex v1 = g_vertices[triangleIndices.y];
    Vertex v2 = g_vertices[triangleIndices.z];

    SubMeshData subMeshData = g_subMeshData;
    Material material = g_materials[g_logicalMaterialIndexToPhysical[subMeshData.m_materialIndex]];

    float2 uv = computeUv(v0, v1, v2, attr);

    uint normalTexIndex = material.m_normalTextureIndex;

    float3 normalTex = float3(0,0,1);

    if (normalTexIndex != UINT32_MAX) 
    {
        normalTex = g_textures[normalTexIndex].SampleLevel(g_sampler, uv, 0).xyz;
        normalTex = normalize(normalTex * 2.0 - 1.0); // [0,1] → [-1,1]
    }

    float3x4 worldMat3x4 = ObjectToWorld3x4();

    float3x3 TBN = computeTBN(v0, v1, v2, attr);

    float3 normalLocal = normalize(mul(normalTex, TBN));
    float3 normal = normalize(mul((float3x3)worldMat3x4, normalLocal));
    float3 weight = mainRayPayloadGetWeight(payload);

    float3 finalColor = normal * weight;
    float3 currentColor = mainRayPayloadGetColor(payload);
    finalColor = finalColor + currentColor;

    mainRayPayloadSetColor(payload, finalColor);
}

inline void handleCameraRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    resolveNormalIntegrator(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    shadowRayPayloadSetInstanceId(payload, InstanceID());
}

inline void handleNearestRay(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
}

[shader("closesthit")]
void NormalClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_MAIN:
            handleCameraRay(payload, attr);
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


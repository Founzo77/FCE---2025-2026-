#include "../common/global_signature.hlsli"
#include "../common/surfacique_local_signature.hlsli"
#include "../common/maths.hlsli"

inline float3 evalNormal(in float3 hitPosition, in float3 normal, in float3 albedo,
    in float3 ks, in float shininess, in RayPayload oldPayload)
{
    uint nbLights = g_constantInformations.m_nbLights;

    float3 resultColor = 0;

    // TO_DO mettre le ambientColor dans la scene ou le materiau
    float3 ambientColor = float3(0.2, 0.2, 0.2);
    resultColor += ambientColor * albedo.rgb;

    float3 cameraPosition = g_constantInformations.m_position.xyz;

    for (int i = 0; i < nbLights; i++)
    {
        Light light = g_lights[i];

        float3 toLight = light.m_position - hitPosition;
        float distanceToLight = length(toLight);
        float3 directionToLight = normalize(toLight);

        RayDesc ray;
        ray.Origin = hitPosition + normal * 1e-3;
        ray.Direction = directionToLight;
        ray.TMin = 0.0f;
        ray.TMax = distanceToLight - 1e-3f;

        RayPayload payload;
        payload.m_type = RAY_TYPE_SHADOW;
        shadowRayPayloadSetMaxHitDistance(payload, distanceToLight - 1e-3f);
        shadowRayPayloadSetWeight(payload, float3(1, 1, 1));
        shadowRayPayloadSetNbBounds(payload, secondaryRayPayloadGetNbBounds(oldPayload) + 1);

        // TO_DO remplacer RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES par RAY_FLAG_NONE
        TraceRay(g_scene, RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 0xFF, 0, 1, 0, ray, payload);

        if (shadowRayPayloadGetInstanceId(payload) == UINT32_MAX) // Light
        {
            float NdotL = max(dot(normal, directionToLight), 0.0f);
            //resultColor += albedo.rgb * light.m_radiance * NdotL;
            float3 radianceLight = light.m_radiance * shadowRayPayloadGetWeight(payload);

            float3 V = normalize(cameraPosition - hitPosition);

            // Diffuse
            float3 diffuse = albedo.rgb * radianceLight * NdotL;

            // Specular (Phong)
            float3 R = reflect(-directionToLight, normal);
            float RdotV = max(dot(R, V), 0.0f);

            float3 specular = ks * pow(RdotV, shininess) * radianceLight;

            resultColor += diffuse + specular;
        }
    }

    return resultColor;
}

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
    float3 albedo = normal;

    float3 hitLocal = computeHitPosition(v0, v1, v2, attr);
    float3 hitPosition = mul(worldMat3x4, float4(hitLocal, 1.0f));

    float3 weight = secondaryRayPayloadGetWeight(payload);

    float3 directColor = 0;
    if(computeRadiance(albedo) != 0)
    {
        directColor = evalNormal(
            hitPosition, normal, albedo, material.m_ks, material.m_shininess, payload) * weight;
    }
    uint nbBounds = secondaryRayPayloadGetNbBounds(payload);

    float3 finalColor = directColor;

    if(nbBounds < MAX_RAY_NB_BOUNDS)
    {
        float3 reflectanceWeight = material.m_reflectance * weight;

        if(computeRadiance(reflectanceWeight) >= MIN_REFLECTANCE_WEIGHT_FOR_BOUND)
        {
            RayDesc secondaryRayDesc;
            secondaryRayDesc.Origin = hitPosition;
            secondaryRayDesc.Direction = reflect(WorldRayDirection(), normal);
            secondaryRayDesc.TMin = 0.001f;
            secondaryRayDesc.TMax = 10000.0f;
        
            RayPayload secondaryPayload;
            secondaryPayload.m_type = RAY_TYPE_SECONDARY;
            secondaryRayPayloadSetNbBounds(secondaryPayload, nbBounds + 1);
            secondaryRayPayloadSetWeight(secondaryPayload, reflectanceWeight);
            secondaryRayPayloadSetColor(secondaryPayload, float3(0, 0, 0));

            TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, secondaryRayDesc, secondaryPayload);

            finalColor += secondaryRayPayloadGetColor(secondaryPayload);
        }

        float3 transmittanceWeight = material.m_transmittance * weight;

        if(computeRadiance(transmittanceWeight) >= MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND)
        {
            RayDesc secondaryRayDesc;
            secondaryRayDesc.Origin = hitPosition;
            secondaryRayDesc.Direction = WorldRayDirection();
            secondaryRayDesc.TMin = 0.001f;
            secondaryRayDesc.TMax = 10000.0f;
        
            RayPayload secondaryPayload;
            secondaryPayload.m_type = RAY_TYPE_SECONDARY;
            secondaryRayPayloadSetNbBounds(secondaryPayload, nbBounds + 1);
            secondaryRayPayloadSetWeight(secondaryPayload, transmittanceWeight);
            secondaryRayPayloadSetColor(secondaryPayload, float3(0, 0, 0));

            TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, secondaryRayDesc, secondaryPayload);

            finalColor += secondaryRayPayloadGetColor(secondaryPayload);
        }
    }

    float3 currentColor = secondaryRayPayloadGetColor(payload);
    finalColor = finalColor + currentColor;
    cameraRayPayloadSetColor(payload, finalColor);
}

inline void handleCameraRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    resolveNormalIntegrator(payload, attr);
}

inline void handleSecondaryRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    resolveNormalIntegrator(payload, attr);
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
void NormalClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_CAMERA:
            handleCameraRay(payload, attr);
            break;
        case RAY_TYPE_SECONDARY:
            handleSecondaryRay(payload, attr);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowRay(payload, attr);
            break;
        case RAY_TYPE_NEAREST:
            handleNearestRay(payload, attr);
            break;
        default:
            cameraRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
}


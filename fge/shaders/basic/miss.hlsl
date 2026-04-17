#include "../common/global_signature.hlsli"
#include "../common/maths.hlsli"

float2 directionToLatLong(float3 dir)
{
    dir = normalize(dir);

    float u = atan2(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = acos(clamp(dir.y, -1.0, 1.0)) / PI;

    return float2(u, v);
}

float3 sampleBackground(float3 rayDir)
{
    uint index = g_constantInformations.m_backgroundTextureIndex;

    if (index == UINT32_MAX)
    {
        return float3(0.0, 0.02, 0.17);
        //return float3(1.0, 0.4156, 0);  
    }

    float2 uv = directionToLatLong(rayDir);

    return g_textures[index].SampleLevel(g_sampler, uv, 0).rgb;
}

inline void handleCameraMiss(inout RayPayload payload)
{
    float3 currentColor = mainRayPayloadGetColor(payload);
    float3 weight = mainRayPayloadGetWeight(payload);
    
    float3 background = sampleBackground(WorldRayDirection());

    float3 finalColor = currentColor + background * weight;
    
    mainRayPayloadSetColor(payload, finalColor);
}

inline void handleShadowMiss(inout RayPayload payload)
{
    shadowRayPayloadSetInstanceId(payload, UINT32_MAX);
}

inline void handleNearestRay(inout RayPayload payload)
{
    nearestRayPayloadSetInstanceId(payload, UINT32_MAX);
}

[shader("miss")]
void MissShader(inout RayPayload payload)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_MAIN:
            handleCameraMiss(payload);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowMiss(payload);
            break;
        case RAY_TYPE_NEAREST:
            handleNearestRay(payload);
            break;
        default:
            mainRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
}

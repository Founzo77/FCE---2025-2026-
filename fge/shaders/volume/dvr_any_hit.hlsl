#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

inline float computeRadiance(in float3 color)
{
    return (color.x + color.y + color.z) / 3; 
} 

inline void resolveDvrIntegrator(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    float3x4 worldToObject = WorldToObject3x4();

    float3 roWorld = WorldRayOrigin();
    float3 rdWorld = WorldRayDirection();

    float3 roObj = mul(worldToObject, float4(roWorld, 1.0));
    float3 rdObj = mul(worldToObject, float4(rdWorld, 0.0));

    float tEntry = max(attr.tEntry, RayTMin());
    float tExit = attr.tExit;

    float t = tEntry;

    RayQuery<RAY_FLAG_NONE> rayQuery;

    RayDesc exitTestRay;
    exitTestRay.Origin = roWorld + rdWorld * t;
    exitTestRay.Direction = rdWorld;
    exitTestRay.TMin = 0.001;
    exitTestRay.TMax = (tExit - t);

    rayQuery.TraceRayInline(g_scene, 
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 
        0xFF, exitTestRay);

    while(rayQuery.Proceed())
    {

    }

    if(rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        tExit = rayQuery.CommittedRayT() + t;
    }

    const int MAX_STEPS = 128;

    float stepSize = (tExit - tEntry) / (float) MAX_STEPS;
    float stepSizeNorm = 1 / (float) MAX_STEPS;

    float3 accumColor = float3(0,0,0);
    float accumAlpha = 0.0;

    uint texIndex = g_volumeData.m_texture3DIndex;

    float scalarMin = g_volumeData.m_scalarMin;
    float scalarMax = g_volumeData.m_scalarMax;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        if (t > tExit || accumAlpha > 0.99)
            break;

        //float3 posObj = roObj + rdObj * t;
        //float3 uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

        float3 posWorld = roWorld + rdWorld * t;
        float3 posObj = mul(worldToObject, float4(posWorld,1)).xyz;
        float3 uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

        float densityRaw = g_textures3D[texIndex].SampleLevel(g_sampler, uvw, 0);
        densityRaw = clamp(densityRaw, scalarMin, scalarMax);
        //float density = saturate((densityRaw - scalarMin) / (scalarMax - scalarMin));
        float density = densityRaw;

        float alpha = 1.0 - exp(-density * stepSize);
        float3 color = density.xxx;

        if(g_volumeData.m_transferFunctionTextureIndex != UINT32_MAX)
        {
            float4 value = g_textures[g_volumeData.m_transferFunctionTextureIndex].
                SampleLevel(g_sampler, float2(density, 0.5f), 0.0f);
            //alpha = value.a;
            color = value.rgb;
            //alpha = 1.0 - pow(1 - alpha, stepSizeNorm);
        }

        accumColor = accumColor + (1.0 - accumAlpha) * alpha * color;
        accumAlpha = accumAlpha + (1.0 - accumAlpha) * alpha;

        /*
        float alpha = 1.0 - exp(-density * stepSize);
        float3 color = density.xxx;

        if(g_volumeData.m_transferFunctionTextureIndex != UINT32_MAX)
        {
            float4 value = g_textures[g_volumeData.m_transferFunctionTextureIndex].
                SampleLevel(g_sampler, float2(0.5f, density), 0.0f);
            alpha = value.a;
            color = value.rgb;
            alpha = 1.0 - exp(-alpha * stepSize);
        }

        accumColor += (1.0 - accumAlpha) * alpha * color;
        accumAlpha += (1.0 - accumAlpha) * alpha;
        */

        t += stepSize;
    }

    float3 weight = mainRayPayloadGetWeight(payload);

    float3 finalColor = accumColor * weight;
    float3 transmittanceWeight = (1.0 - accumAlpha) * weight;

    float3 currentColor = mainRayPayloadGetColor(payload);
    finalColor = finalColor + currentColor;
    
    mainRayPayloadSetColor(payload, finalColor);
    mainRayPayloadSetWeight(payload, transmittanceWeight);

    // TO_DO Mettre 0.99 en CONSTANTE
    bool isOpaque = computeRadiance(transmittanceWeight) < MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND;

    if (isOpaque)
    {
        AcceptHitAndEndSearch(); // IMPORTANT: commit le hit
    }
    else
    {
        IgnoreHit(); // continue pour accumuler / traverser
    }
}

inline void handleCameraRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    resolveDvrIntegrator(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    IgnoreHit();
}

inline void handleNearestRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    // TO_DO voir si on fait autre chose

    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
    IgnoreHit();
}

[shader("anyhit")]
void DvrAnyHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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

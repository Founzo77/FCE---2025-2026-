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

    RayDesc exitTestRay;
    exitTestRay.Origin = roWorld + rdWorld * t;
    exitTestRay.Direction = rdWorld;
    exitTestRay.TMin = 0.001;
    exitTestRay.TMax = (tExit - t);

    uint nbBounds = mainRayPayloadGetNbBounds(payload);

    RayPayload exitTestPayload;
    exitTestPayload.m_type = RAY_TYPE_NEAREST;
    nearestRayPayloadSetNbBounds(exitTestPayload, nbBounds + 1);
    
    TraceRay(g_scene, 
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES,
        0xFF, 0, 1, 0, exitTestRay, exitTestPayload);
    
    if(nearestRayPayloadGetInstanceId(exitTestPayload) != UINT32_MAX)
    {
        tExit = t + nearestRayPayloadGetHitDistance(exitTestPayload);
    }

    const int MAX_STEPS = 128;

    float stepSize = (tExit - tEntry) / MAX_STEPS;

    float3 accumColor = float3(0,0,0);
    float accumAlpha = 0.0;

    uint texIndex = g_volumeData.m_texture3DIndex;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        if (t > tExit || accumAlpha > 0.99)
            break;

        float3 posObj = roObj + rdObj * t;

        float3 uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

        if (any(uvw < 0.0) || any(uvw > 1.0))
        {
            t += stepSize;
            continue;
        }

        float densityRaw = g_textures3D[texIndex].SampleLevel(g_sampler, uvw, 0);
        float densityTmp = lerp(g_volumeData.m_scalarMin, g_volumeData.m_scalarMax, densityRaw);
        float density = saturate((densityTmp - g_volumeData.m_scalarMin) 
            / (g_volumeData.m_scalarMax - g_volumeData.m_scalarMin));

        float alpha = 1.0 - exp(-density * stepSize);
        float3 color = density.xxx;

        if(g_volumeData.m_transferFunctionTextureIndex != UINT32_MAX)
        {
            float4 value = g_textures[g_volumeData.m_transferFunctionTextureIndex].
                SampleLevel(g_sampler, float2(density, 0.5f), 0.0f);
            //alpha = value.a;
            alpha = value.a * stepSize;
            color = value.rgb;
            //alpha = 1.0 - exp(-alpha * stepSize);
        }

        accumColor += (1.0 - accumAlpha) * alpha * color;
        accumAlpha += (1.0 - accumAlpha) * alpha;

        t += stepSize;
    }

    float3 weight = mainRayPayloadGetWeight(payload);

    float3 finalColor = accumColor * weight;
    float3 transmittanceWeight = (1.0 - accumAlpha) * weight;

    // TO_DO Mettre 0.99 en CONSTANTE
    if(computeRadiance(transmittanceWeight) >= MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND 
        && nbBounds < MAX_RAY_NB_BOUNDS) // Pas opaque
    {
        if(nearestRayPayloadGetInstanceId(exitTestPayload) != UINT32_MAX 
            && nbBounds < MAX_RAY_NB_BOUNDS)
        {
            // On va toucher la surface dans le volume

            RayPayload transmittancePayload;
            transmittancePayload.m_type = RAY_TYPE_MAIN;
            mainRayPayloadSetNbBounds(transmittancePayload, nbBounds + 1);
            mainRayPayloadSetWeight(transmittancePayload, transmittanceWeight);
            mainRayPayloadSetColor(transmittancePayload, float3(0, 0, 0));

            TraceRay(g_scene, 
                // TO_DO Voir si peut etre retire RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH comme
                // plus besoin
                RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 
                0xFF, 0, 1, 0, exitTestRay, transmittancePayload);

            finalColor += mainRayPayloadGetColor(transmittancePayload);
        }
        else
        {
            // On va toucher la surface derriere le volume

            RayDesc transmittanceRay;
            transmittanceRay.Origin = roWorld + rdWorld * tExit;
            transmittanceRay.Direction =rdWorld;
            transmittanceRay.TMin = 0.001f;
            transmittanceRay.TMax = 10000.0f;

            RayPayload transmittancePayload;
            transmittancePayload.m_type = RAY_TYPE_MAIN;
            mainRayPayloadSetNbBounds(transmittancePayload, nbBounds + 1);
            mainRayPayloadSetWeight(transmittancePayload, transmittanceWeight);
            mainRayPayloadSetColor(transmittancePayload, float3(0, 0, 0));

            TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, transmittanceRay, transmittancePayload);

            finalColor += mainRayPayloadGetColor(transmittancePayload);
        }
    }
    
    mainRayPayloadSetColor(payload, finalColor);
}

inline void handleCameraRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    resolveDvrIntegrator(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{

}

inline void handleNearestRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
}

[shader("closesthit")]
void DvrClosestHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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

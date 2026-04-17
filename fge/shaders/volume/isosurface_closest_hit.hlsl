#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

inline float computeRadiance(in float3 color)
{
    return (color.x + color.y + color.z) / 3; 
} 

inline float sampleVolume(in float3 position)
{
    float3 uvw = (position - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);
    return g_textures3D[g_volumeData.m_texture3DIndex].SampleLevel(g_sampler, uvw, 0);
}

inline void resolveIsosurfaceIntegrator(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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
    const float DENSITY_SCALE = 1;
    const float RHO_ISO = 0.01; // Bunny

    float stepSize = (tExit - tEntry) / MAX_STEPS;

    float3 posObj = roObj + rdObj * t;
    float3 uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

    uint textureIndex = g_volumeData.m_texture3DIndex;
    float rhoPrevious = g_textures3D[textureIndex].SampleLevel(g_sampler, uvw, 0) * DENSITY_SCALE;
    float tPrevious = t;

    t += stepSize;

    float tIso = 0;
    bool isIso = false;

    for (int i = 1; i < MAX_STEPS; i++)
    {
        if (t > tExit)
            break;

        posObj = roObj + rdObj * t;
        uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

        float rho = g_textures3D[textureIndex].SampleLevel(g_sampler, uvw, 0) * DENSITY_SCALE;

        if((rhoPrevious - RHO_ISO) * (rho - RHO_ISO) < 0)
        {
            tIso = tPrevious + (RHO_ISO - rhoPrevious) / (rho - rhoPrevious) * stepSize;
            isIso = true;

            break;
        }

        tPrevious = t;
        rhoPrevious = rho;
        t += stepSize;
    }

    float3 finalColor = float3(0, 0, 0);
    float alpha = 0;

    if(isIso)
    {
        float3 positionIsoObj = roObj + rdObj * tIso;

        float3 gradient;

        gradient.x = sampleVolume(positionIsoObj + float3(stepSize, 0, 0)) * DENSITY_SCALE
            - sampleVolume(positionIsoObj - float3(stepSize, 0, 0)) * DENSITY_SCALE;
        gradient.y = sampleVolume(positionIsoObj + float3(0, stepSize, 0)) * DENSITY_SCALE
            - sampleVolume(positionIsoObj - float3(0, stepSize, 0)) * DENSITY_SCALE;
        gradient.z = sampleVolume(positionIsoObj + float3(0, 0, stepSize)) * DENSITY_SCALE
            - sampleVolume(positionIsoObj - float3(0, 0, stepSize)) * DENSITY_SCALE;

        float3 normale = normalize(gradient);
        //if (dot(normale, rdObj) > 0)
        //    normale = -normale;

        //finalColor = float3(1, 0, 0);
        finalColor = normale;
        alpha = 1;
    }

    float3 weight = mainRayPayloadGetWeight(payload);

    float3 transmittanceWeight = (1.0 - alpha) * weight;

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
    resolveIsosurfaceIntegrator(payload, attr);
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
void IsosurfaceClosestHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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

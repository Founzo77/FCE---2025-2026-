#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

inline float computeRadiance(in float3 color)
{
    return (color.x + color.y + color.z) / 3;
}

inline void resolveParticleIntegrator(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_FORCE_OPAQUE |RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 
        0xFF, exitTestRay);

    while(rayQuery.Proceed())
    {

    }

    if(rayQuery.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        tExit = min(tExit, rayQuery.CommittedRayT() + t);
    }

    uint nbBounds = mainRayPayloadGetNbBounds(payload);

    const int MAX_STEPS = 128;

    float stepSize = (tExit - tEntry) / MAX_STEPS;

    float3 posObj = roObj + rdObj * t;
    float3 uvw = (posObj - g_volumeData.m_mins) / (g_volumeData.m_maxs - g_volumeData.m_mins);

    //float3 baseColor = float3(0.5, 0, 0);
    float3 baseColor = float3(0.2, 0.2, 0.2);

    float3 weight = mainRayPayloadGetWeight(payload);

    float3 finalColor = baseColor * weight;
    
    float alpha = 0.5;
    float3 transmittanceWeight = (1.0 - alpha) * weight;

    float3 currentColor = mainRayPayloadGetColor(payload);
    finalColor = finalColor + currentColor;

    mainRayPayloadSetColor(payload, finalColor);
    mainRayPayloadSetWeight(payload, transmittanceWeight);

    // TO_DO Mettre 0.99 en CONSTANTE
    bool isOpaque =
        (computeRadiance(transmittanceWeight) < MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND) ||
        (nbBounds >= MAX_RAY_NB_BOUNDS);

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
    resolveParticleIntegrator(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    IgnoreHit();
}

inline void handleNearestRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    IgnoreHit();
}

[shader("anyhit")]
void ParticleAnyHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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
            IgnoreHit();
            break;
    }
}

#ifndef __SURFACE_PHONG_INTEGRATOR_H__
#define __SURFACE_PHONG_INTEGRATOR_H__

#include "../../common/data.hlsli"
#include "../../common/global_signature.hlsli"
#include "../../common/surfacique_local_signature.hlsli"
#include "../../common/maths.hlsli"

#include "../../surface/surface.hlsli"

bool phongHasNextLight(in LightAccumulator lightAccumulator);
void phongIterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample);
void phongSampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample);
void phongSampleBsdf(in Intersection intersection, inout LightSample lightSample);

void phongIterateNextRay(inout RayPayload rayPayload, in Intersection intersection);

bool phongHasNextLight(in LightAccumulator lightAccumulator)
{
    return lightAccumulator.m_sampleIndex < g_constantInformations.m_nbLights;
}
void phongIterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample)
{
    lightSample.m_lightIndex = lightAccumulator.m_sampleIndex;
    lightAccumulator.m_sampleIndex++;
    lightAccumulator.m_weight = mainRayPayloadGetWeight(rayPayload);
}
void phongSampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample)
{
    uint lightIndex = lightSample.m_lightIndex;
    Light light = g_lights[lightIndex];

    float3 hitPosition = intersection.m_worldPosition;

    float3 toLight = light.m_position - hitPosition;
    float distanceToLight = length(toLight);
    float3 directionToLight = normalize(toLight);

    // wo = direction vers la lumière
    lightSample.m_worldWo = directionToLight;

    RayDesc ray;
    ray.Origin = hitPosition + directionToLight * 1e-3;
    ray.Direction = directionToLight;
    ray.TMin = 0.0f;
    ray.TMax = distanceToLight - 1e-3f;

    RayPayload payload;
    payload.m_type = RAY_TYPE_SHADOW;

    shadowRayPayloadSetInstanceId(payload, UINT32_MAX);
    shadowRayPayloadSetMaxHitDistance(payload, distanceToLight - 1e-3f);
    shadowRayPayloadSetWeight(payload, float3(1,1,1));
    shadowRayPayloadSetNbBounds(payload, mainRayPayloadGetNbBounds(rayPayload) + 1);

    TraceRay(g_scene, RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 0xFF, 0, 1, 0, ray, payload);

    if (shadowRayPayloadGetInstanceId(payload) == UINT32_MAX)
    {
        lightSample.m_radiance = light.m_radiance * shadowRayPayloadGetWeight(payload);
    }
    else
    {
        lightSample.m_radiance = float3(0,0,0);
    }
}

void phongSampleBsdf(in Intersection intersection, inout LightSample lightSample)
{
    float3 tbnWo = worldToTbn(intersection.m_tbn, lightSample.m_worldWo);
    float3 tbnWi = worldToTbn(intersection.m_tbn, intersection.m_worldWi);

    float3 f;
    bsdfEval(intersection, tbnWo, f, SURFACE_LOBE_DIFFUSE);

    lightSample.m_surfaceColor = f * lightSample.m_radiance;
}

void phongIterateNextRay(inout RayPayload rayPayload, in Intersection intersection)
{
    uint nbBounds = mainRayPayloadGetNbBounds(rayPayload);

    if(nbBounds >= MAX_RAY_NB_BOUNDS)
        return;

    float3 hitPosition = intersection.m_worldPosition;

    float3 weight = mainRayPayloadGetWeight(rayPayload);

    float3 accumulatedColor = float3(0, 0, 0);

    // reflection
    float3 tbnWo;
    float pdf;
    bsdfSample(rayPayload, intersection, tbnWo, pdf, SURFACE_LOBE_DIRAC_REFLECTANCE);
    float3 wo = tbnToWorld(intersection.m_tbn, tbnWo);

    float3 reflectanceWeight = float3(0, 0, 0);
    bsdfEval(intersection, tbnWo, reflectanceWeight, SURFACE_LOBE_DIRAC_REFLECTANCE);
    reflectanceWeight = reflectanceWeight * weight;

    if(computeRadiance(reflectanceWeight) >= MIN_REFLECTANCE_WEIGHT_FOR_BOUND)
    {
        RayDesc rayDesc;
        rayDesc.Origin = hitPosition;
        rayDesc.Direction = wo;
        rayDesc.TMin = 0.001f;
        rayDesc.TMax = 10000.0f;

        RayPayload secondaryPayload;
        secondaryPayload.m_type = RAY_TYPE_MAIN;

        mainRayPayloadSetNbBounds(secondaryPayload, nbBounds + 1);
        mainRayPayloadSetWeight(secondaryPayload, reflectanceWeight);
        mainRayPayloadSetColor(secondaryPayload, float3(0,0,0));

        TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, rayDesc, secondaryPayload);

        accumulatedColor += mainRayPayloadGetColor(secondaryPayload);
    }

    // transmission
    bsdfSample(rayPayload, intersection, tbnWo, pdf, SURFACE_LOBE_DIRAC_TRANSMITTANCE);
    wo = tbnToWorld(intersection.m_tbn, tbnWo);

    float3 transmittanceWeight = float3(0, 0, 0);
    bsdfEval(intersection, tbnWo, transmittanceWeight, SURFACE_LOBE_DIRAC_TRANSMITTANCE);
    transmittanceWeight = transmittanceWeight * weight;

    if(computeRadiance(transmittanceWeight) >= MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND)
    {
        RayDesc rayDesc;
        rayDesc.Origin = hitPosition;
        rayDesc.Direction = wo;
        rayDesc.TMin = 0.001f;
        rayDesc.TMax = 10000.0f;

        RayPayload secondaryPayload;
        secondaryPayload.m_type = RAY_TYPE_MAIN;

        mainRayPayloadSetNbBounds(secondaryPayload, nbBounds + 1);
        mainRayPayloadSetWeight(secondaryPayload, transmittanceWeight);
        mainRayPayloadSetColor(secondaryPayload, float3(0,0,0));

        TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, rayDesc, secondaryPayload);

        accumulatedColor += mainRayPayloadGetColor(secondaryPayload);
    }

    float3 current = mainRayPayloadGetColor(rayPayload);
    mainRayPayloadSetColor(rayPayload, current + accumulatedColor);
}

#endif
#ifndef __SURFACE_PATHTRACER_INTEGRATOR_H__
#define __SURFACE_PATHTRACER_INTEGRATOR_H__

#include "../../common/data.hlsli"
#include "../../common/global_signature.hlsli"
#include "../../common/surfacique_local_signature.hlsli"
#include "../../common/maths.hlsli"

#include "../../surface/surface.hlsli"

#define PATHTRACER_NB_LIGHTS 10
#define PATHTRACER_NB_BOUNDS 5

bool pathtracerHasNextLight(in LightAccumulator lightAccumulator);
void pathtracerIterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample);
void pathtracerSampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample);
void pathtracerSampleBsdf(in Intersection intersection, inout LightSample lightSample);

void pathtracerIterateNextRay(inout RayPayload rayPayload, in Intersection intersection);

bool pathtracerHasNextLight(in LightAccumulator lightAccumulator)
{
    uint nbMaxLight = min(PATHTRACER_NB_LIGHTS, g_constantInformations.m_nbLights);

    return lightAccumulator.m_sampleIndex < nbMaxLight;
}
void pathtracerIterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample)
{
    // pick une lumière aléatoire
    uint lightIndex = uint(
        randomFloat(rayPayload) * g_constantInformations.m_nbLights);
    lightIndex = clamp(lightIndex, 0, g_constantInformations.m_nbLights - 1);

    lightSample.m_lightIndex = lightIndex;

    lightAccumulator.m_sampleIndex++;
    lightAccumulator.m_weight = mainRayPayloadGetWeight(rayPayload);
}
void pathtracerSampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample)
{
    Light light = g_lights[lightSample.m_lightIndex];

    float3 hitPosition = intersection.m_worldPosition;

    float3 toLight = light.m_position - hitPosition;
    float dist = length(toLight);
    float3 dir = normalize(toLight);

    lightSample.m_worldWo = dir;

    // shadow ray
    RayDesc ray;
    ray.Origin = hitPosition + dir * 1e-3;
    ray.Direction = dir;
    ray.TMin = 0.0f;
    ray.TMax = dist - 1e-3;

    RayPayload payload;
    payload.m_type = RAY_TYPE_SHADOW;

    shadowRayPayloadSetInstanceId(payload, UINT32_MAX);
    shadowRayPayloadSetMaxHitDistance(payload, dist - 1e-3);
    shadowRayPayloadSetWeight(payload, float3(1,1,1));
    shadowRayPayloadSetNbBounds(payload, mainRayPayloadGetNbBounds(rayPayload) + 1);

    TraceRay(g_scene, RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES, 0xFF, 0, 1, 0, ray, payload);

    if (shadowRayPayloadGetInstanceId(payload) == UINT32_MAX)
    {
        uint nbLights = g_constantInformations.m_nbLights;
        uint nbSamples = min(PATHTRACER_NB_LIGHTS, nbLights);
        float normalization = float(nbLights) / float(nbSamples);

        lightSample.m_radiance = light.m_radiance * normalization;;
    }
    else
    {
        lightSample.m_radiance = float3(0,0,0);
    }
}

void pathtracerSampleBsdf(in Intersection intersection, inout LightSample lightSample)
{
    float3 tbnWo = worldToTbn(intersection.m_tbn, lightSample.m_worldWo);

    float3 f;
    bsdfEval(intersection, tbnWo, f, SURFACE_LOBE_ALL);

    lightSample.m_surfaceColor = f * lightSample.m_radiance;
}

void pathtracerIterateNextRay(inout RayPayload rayPayload, in Intersection intersection)
{
    uint nbBounds = mainRayPayloadGetNbBounds(rayPayload);
    if (nbBounds >= PATHTRACER_NB_BOUNDS)
        return;

    float3 hitPosition = intersection.m_worldPosition;

    float3 weight = mainRayPayloadGetWeight(rayPayload);

    float2 rand = randomFloat2(rayPayload);

    float3 tbnWo;
    float pdf;
    bsdfSample(rayPayload, intersection, tbnWo, pdf, SURFACE_LOBE_ALL);
    float3 wo = tbnToWorld(intersection.m_tbn, tbnWo);

    float3 wi = intersection.m_worldWi;

    float3 f;
    bsdfEval(intersection, tbnWo, f, SURFACE_LOBE_ALL);

    float3 newWeight = weight * f / max(pdf, 1e-6);

    // Russian roulette (simple)
    float p = saturate(max(max(newWeight.x, newWeight.y), newWeight.z));

    if (p < 0.01 || randomFloat(rayPayload) > p)
    {
        return;
    }

    newWeight /= p;

    RayDesc ray;
    ray.Origin = hitPosition + wo * 1e-3;
    ray.Direction = wo;
    ray.TMin = 0.001f;
    ray.TMax = 10000.0f;

    RayPayload newPayload;
    newPayload.m_type = RAY_TYPE_MAIN;

    uint rngSeed = randomUint(rayPayload);

    mainRayPayloadSetNbBounds(newPayload, nbBounds + 1);
    mainRayPayloadSetRngSeed(newPayload, rngSeed);
    mainRayPayloadSetWeight(newPayload, newWeight);
    mainRayPayloadSetColor(newPayload, float3(0,0,0));

    TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, ray, newPayload);

    float3 current = mainRayPayloadGetColor(rayPayload);
    mainRayPayloadSetColor(rayPayload, current + mainRayPayloadGetColor(newPayload));
}

#endif
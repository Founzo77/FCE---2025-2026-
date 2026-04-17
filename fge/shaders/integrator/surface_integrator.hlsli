#ifndef __SURFACE_INTEGRATOR_H__
#define __SURFACE_INTEGRATOR_H__

#include "../common/data.hlsli"
#include "../common/global_signature.hlsli"
#include "../common/surfacique_local_signature.hlsli"
#include "../common/maths.hlsli"

#include "../surface/surface.hlsli"

#include "phong/surface_phong_integrator.hlsli"
#include "pathtracer/surface_pathtracer_integrator.hlsli"

void onClosestHitSurface(inout RayPayload rayPayload, 
    in BuiltInTriangleIntersectionAttributes attr);

void initializeIntersection(in BuiltInTriangleIntersectionAttributes attr,
    out Intersection intersection);
void initializeLightAccumulator(in RayPayload rayPayload,
    out LightAccumulator lightAccumulator);

bool hasNextLight(in LightAccumulator lightAccumulator);
void iterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample);
void sampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample);
void sampleBsdf(in Intersection intersection, inout LightSample lightSample);
void addLightSample(in out LightAccumulator lightAccumulator, in LightSample lightSample);

void addIntersectionSample(inout RayPayload rayPayload, in LightAccumulator lightAccumulator);
bool hasNextRay(in RayPayload rayPayload);
void iterateNextRay(inout RayPayload rayPayload, in Intersection intersection);

void onClosestHitSurface(inout RayPayload rayPayload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    Intersection intersection;
    initializeIntersection(attr, intersection);

    LightAccumulator lightAccumulator;
    initializeLightAccumulator(rayPayload, lightAccumulator);

    while(hasNextLight(lightAccumulator) == true)
    {
        LightSample lightSample;
        iterateNextLight(rayPayload, intersection, lightAccumulator, lightSample);

        sampleLight(rayPayload, intersection, lightSample);
        sampleBsdf(intersection, lightSample);

        addLightSample(lightAccumulator, lightSample);
    }

    addIntersectionSample(rayPayload, lightAccumulator);

    iterateNextRay(rayPayload, intersection);
}

void initializeIntersection(in BuiltInTriangleIntersectionAttributes attr,
    out Intersection intersection)
{
    uint idTriangle = PrimitiveIndex();
    uint3 triangleIndices = g_indices[idTriangle];

    Vertex v0Local = g_vertices[triangleIndices.x];
    Vertex v1Local = g_vertices[triangleIndices.y];
    Vertex v2Local = g_vertices[triangleIndices.z];

    float3x4 worldMat3x4 = ObjectToWorld3x4();

    Vertex v0 = localToWorld(v0Local, worldMat3x4);
    Vertex v1 = localToWorld(v1Local, worldMat3x4);
    Vertex v2 = localToWorld(v2Local, worldMat3x4);

    float2 uv = computeUv(v0, v1, v2, attr);

    float3 normalTex = float3(0, 0, 1);
    surfaceGetNormalDeformation(uv, normalTex);

    float3x3 TBN = computeTBN(v0, v1, v2, attr);

    //float3 normal = tbnToWorld(TBN, normalTex);

    float3 hitPosition = computeHitPosition(v0, v1, v2, attr);

    intersection.m_worldPosition = hitPosition;
    // TO_DO Voir si normal ou shadingNormal
    intersection.m_tbnShadingNormal = normalTex;
    intersection.m_uvw = float3(uv.x, uv.y, 0.0f);
    intersection.m_worldWi = -WorldRayDirection();
    intersection.m_tbn = TBN;
}
void initializeLightAccumulator(in RayPayload rayPayload,
    out LightAccumulator lightAccumulator)
{
    lightAccumulator.m_accumulatedRadiance = float3(0,0,0);
    lightAccumulator.m_sampleIndex = 0;
    lightAccumulator.m_weight = mainRayPayloadGetWeight(rayPayload);
}

bool hasNextLight(in LightAccumulator lightAccumulator)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            return phongHasNextLight(lightAccumulator);
        }

        case INTEGRATOR_PATHTRACER:
        {
            return pathtracerHasNextLight(lightAccumulator);
        }

        default:
        {
            return false;
        }
    }

    return false;
}
void iterateNextLight(inout RayPayload rayPayload, in Intersection intersection, 
    inout LightAccumulator lightAccumulator, out LightSample lightSample)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            phongIterateNextLight(rayPayload, intersection, lightAccumulator, lightSample);
            break;
        }

        case INTEGRATOR_PATHTRACER:
        {
            pathtracerIterateNextLight(rayPayload, intersection, lightAccumulator, lightSample);
            break;
        }
    }
}
void sampleLight(in RayPayload rayPayload, in Intersection intersection, 
    inout LightSample lightSample)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            phongSampleLight(rayPayload, intersection, lightSample);
            break;
        }

        case INTEGRATOR_PATHTRACER:
        {
            pathtracerSampleLight(rayPayload, intersection, lightSample);
            break;
        }
    }
}
void sampleBsdf(in Intersection intersection, inout LightSample lightSample)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            phongSampleBsdf(intersection, lightSample);
            break;
        }

        case INTEGRATOR_PATHTRACER:
        {
            pathtracerSampleBsdf(intersection, lightSample);
            break;
        }
    }
}
void addLightSample(in out LightAccumulator lightAccumulator, in LightSample lightSample)
{
    lightAccumulator.m_accumulatedRadiance += lightSample.m_surfaceColor;
}

void addIntersectionSample(inout RayPayload rayPayload, in LightAccumulator lightAccumulator)
{
    float3 before = mainRayPayloadGetColor(rayPayload);
    float3 current = lightAccumulator.m_accumulatedRadiance * lightAccumulator.m_weight;

    mainRayPayloadSetColor(rayPayload, before + current);
}

bool hasNextRay(in RayPayload rayPayload)
{
    return false;
}

void iterateNextRay(inout RayPayload rayPayload, in Intersection intersection)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            phongIterateNextRay(rayPayload, intersection);
            break;
        }

        case INTEGRATOR_PATHTRACER:
        {
            pathtracerIterateNextRay(rayPayload, intersection);
            break;
        }
    }
}

#endif
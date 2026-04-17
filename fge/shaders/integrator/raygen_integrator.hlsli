#ifndef __RAYGEN_INTEGRATOR_H__
#define __RAYGEN_INTEGRATOR_H__

#include "../common/data.hlsli"
#include "../common/global_signature.hlsli"
#include "../common/maths.hlsli"

#include "phong/raygen_phong_integrator.hlsli"
#include "pathtracer/raygen_pathtracer_integrator.hlsli"

void onRayGen(in uint2 pixelIndex, in uint2 pixelDimension);

void initializePixelAccumulator(out PixelAccumulator pixelAccumulator, 
    in uint2 pixelIndex, in uint2 pixelDimension);
void initializeRayDesc(inout PixelAccumulator pixelAccumulator, out RayDesc rayDesc);
void initializeRayPayload(out RayPayload rayPayload, const uint rngSeed);
void spawnRay(in PixelAccumulator pixelAccumulator, 
    in RayDesc rayDesc, inout RayPayload rayPayload);
void addPixelSample(inout PixelAccumulator pixelAccumulator, in RayPayload rayPayload);
bool iterateNextPixelSample(inout PixelAccumulator pixelAccumulator);

void onRayGen(in uint2 pixelIndex, in uint2 pixelDimension)
{
    PixelAccumulator pixelAccumulator;

    initializePixelAccumulator(pixelAccumulator, pixelIndex, pixelDimension);

    while(iterateNextPixelSample(pixelAccumulator) == true)
    {
        RayDesc rayDesc;
        initializeRayDesc(pixelAccumulator, rayDesc);

        RayPayload rayPayload;
        uint rngSeed = randomUint(pixelAccumulator.m_rngSeed);
        rngSeed ^= pixelAccumulator.m_sampleIndex * 747796405u;
        rngSeed = hash(rngSeed);
        initializeRayPayload(rayPayload, rngSeed);

        spawnRay(pixelAccumulator, rayDesc, rayPayload);

        addPixelSample(pixelAccumulator, rayPayload);
    }

    /*
    float3 finalColor = pixelAccumulatorGetFinalColor(pixelAccumulator);
    if (g_constantInformations.m_sppIndex == 0)
    {
        g_accumulatedOutput[pixelIndex] = float4(finalColor, 1);
        g_output[pixelIndex] = float4(finalColor, 1);
        return;
    }
    else
    {
        float3 prev = g_accumulatedInput[pixelIndex].rgb;
        g_accumulatedOutput[pixelIndex] = float4(prev, 1);
        g_output[pixelIndex] = float4(prev, 1);
        return;
    }
    */

    if (g_constantInformations.m_sppIndex > 0)
    {
        pixelAccumulator.m_accumulatedColor += g_accumulatedInput[pixelIndex].rgb;
        pixelAccumulator.m_sampleIndex += g_constantInformations.m_sppIndex;
    }

    g_accumulatedOutput[pixelIndex] = float4(pixelAccumulator.m_accumulatedColor, 1.0);

    float3 finalColor = pixelAccumulatorGetFinalColor(pixelAccumulator);
    g_output[pixelIndex] = float4(finalColor, 1.0);
}

void initializePixelAccumulator(out PixelAccumulator pixelAccumulator,
    in uint2 pixelIndex, in uint2 pixelDimension)
{
    pixelAccumulator.m_accumulatedColor = float3(0, 0, 0);
    pixelAccumulator.m_sampleIndex = 0;

    uint seed = pixelIndex.x;
    seed = seed * 73856093u ^ pixelIndex.y * 19349663u;
    seed ^= g_constantInformations.m_sppIndex * 83492791u;
    pixelAccumulator.m_rngSeed = hash(seed);

    pixelAccumulator.m_pixelIndex = pixelIndex;
    pixelAccumulator.m_pixelDimension = pixelDimension;
}
void initializeRayDesc(inout PixelAccumulator pixelAccumulator, out RayDesc rayDesc)
{
    float2 rng = randomFloat2(pixelAccumulator.m_rngSeed);
    
    // UV + jitter
    float2 uv  = (float2(pixelAccumulator.m_pixelIndex) + rng) / 
        float2(pixelAccumulator.m_pixelDimension);
    uv.y = 1.0 - uv.y;
    float2 ndc = uv * 2.0f - 1.0f;

    float3 imagePoint =
        g_constantInformations.m_position.xyz +
        g_constantInformations.m_forward.xyz * g_constantInformations.m_depth +
        g_constantInformations.m_right.xyz * ndc.x * 
        g_constantInformations.m_physicalWidth * 0.5f +
        g_constantInformations.m_up.xyz * ndc.y * 
        g_constantInformations.m_physicalHeight * 0.5f;

    rayDesc.Origin = g_constantInformations.m_position.xyz;
    rayDesc.Direction = normalize(imagePoint - g_constantInformations.m_position.xyz);
    rayDesc.TMin = 0.001f;
    rayDesc.TMax = 10000.0f;
}
void initializeRayPayload(out RayPayload rayPayload, const uint rngSeed)
{
    rayPayload.m_type = RAY_TYPE_MAIN;
    mainRayPayloadSetNbBounds(rayPayload, 0);
    mainRayPayloadSetRngSeed(rayPayload, rngSeed);
    mainRayPayloadSetWeight(rayPayload, float3(1, 1, 1));
    mainRayPayloadSetColor(rayPayload, float3(0, 0, 0));
}
void spawnRay(in PixelAccumulator pixelAccumulator, 
    in RayDesc rayDesc, inout RayPayload rayPayload)
{
    TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, rayDesc, rayPayload);
}
void addPixelSample(inout PixelAccumulator pixelAccumulator, in RayPayload rayPayload)
{
    pixelAccumulator.m_accumulatedColor += mainRayPayloadGetColor(rayPayload);
}
bool iterateNextPixelSample(inout PixelAccumulator pixelAccumulator)
{
    switch(g_constantInformations.m_integratorType)
    {
        case INTEGRATOR_PHONG:
        {
            return phongIterateNextPixelSample(pixelAccumulator);
        }

        case INTEGRATOR_PATHTRACER:
        {
            return pathtracerIterateNextPixelSample(pixelAccumulator);
        }

        default:
        {
            return false;
        }
    }

    return false;
}

#endif
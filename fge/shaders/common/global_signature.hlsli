#ifndef __GLOBAL_SIGNATURE_H__
#define __GLOBAL_SIGNATURE_H__

#include "data.hlsli"

static const uint UINT32_MAX = 0xFFFFFFFF;

// TO_DO Mettre dans le DeviceConstantInformations
static const uint MAX_RAY_NB_BOUNDS = 10;

static const float MIN_REFLECTANCE_WEIGHT_FOR_BOUND = 0.02;
static const float MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND = 0.02;

cbuffer Camera : register(b0, space0)
{
    DeviceConstantInformations g_constantInformations;
};
RWTexture2D<float4> g_output : register(u0, space0);
RWTexture2D<float4> g_accumulatedInput : register(u1, space0);
RWTexture2D<float4> g_accumulatedOutput : register(u2, space0);
RaytracingAccelerationStructure g_scene : register(t0, space0);
StructuredBuffer<Light> g_lights : register(t1, space0);

StructuredBuffer<Material> g_materials : register(t2, space1);
StructuredBuffer<uint> g_logicalMaterialIndexToPhysical : register(t3, space1);
Texture2D<float4> g_textures[] : register(t4, space1);

Texture3D<float> g_textures3D[] : register(t2, space2);

SamplerState g_sampler : register(s0, space0);

#endif

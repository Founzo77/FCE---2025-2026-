
#ifndef __COMMON_H__
#define __COMMON_H__

static const uint UINT32_MAX = 0xFFFFFFFF;

static const uint RAY_TYPE_CAMERA = 0;
static const uint RAY_TYPE_SECONDARY = 1;
static const uint RAY_TYPE_SHADOW = 2;

// TO_DO Mettre dans le DeviceConstantInformations
static const uint MAX_RAY_NB_BOUNDS = 10;

static const float MIN_REFLECTANCE_WEIGHT_FOR_BOUND = 0.02;
static const float MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND = 0.02;

struct RayPayload
{
    uint m_type;
    uint m_padding[11];
};

/*
struct CameraRayPayload
{
    uint m_type;

    uint m_nbBounds; // Nombre courrants de rebonds avant le traitement du rayon
    uint2 _padding0;
    
    float3 m_color;
    float _padding1;

    float3 m_weight;
    float _padding2;
};
*/

inline uint cameraRayPayloadGetNbBounds(in RayPayload payload)
{
    return asuint(payload.m_padding[0]);
}

inline void cameraRayPayloadSetNbBounds(inout RayPayload payload, const uint nbBounds)
{
    payload.m_padding[0] = nbBounds;
}

inline float3 cameraRayPayloadGetColor(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[3]);
    result.y = asfloat(payload.m_padding[4]);
    result.z = asfloat(payload.m_padding[5]);

    return result;
}

inline void cameraRayPayloadSetColor(inout RayPayload payload, const float3 color)
{
    payload.m_padding[3] = asuint(color.x);
    payload.m_padding[4] = asuint(color.y);
    payload.m_padding[5] = asuint(color.z);
}

inline float3 cameraRayPayloadGetWeight(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[7]);
    result.y = asfloat(payload.m_padding[8]);
    result.z = asfloat(payload.m_padding[9]);

    return result;
}

inline void cameraRayPayloadSetWeight(inout RayPayload payload, const float3 color)
{
    payload.m_padding[7] = asuint(color.x);
    payload.m_padding[8] = asuint(color.y);
    payload.m_padding[9] = asuint(color.z);
}

/*
struct SecondaryRayPayload
{
    uint m_type;
    
    uint m_nbBounds; // Nombre courrants de rebonds avant le traitement du rayon
    uint2 _padding0;
    
    float3 m_color;
    float _padding1;

    float3 m_weight;
    float _padding2;
};
*/

inline uint secondaryRayPayloadGetNbBounds(in RayPayload payload)
{
    return asuint(payload.m_padding[0]);
}

inline void secondaryRayPayloadSetNbBounds(inout RayPayload payload, const uint nbBounds)
{
    payload.m_padding[0] = nbBounds;
}

inline float3 secondaryRayPayloadGetColor(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[3]);
    result.y = asfloat(payload.m_padding[4]);
    result.z = asfloat(payload.m_padding[5]);

    return result;
}

inline void secondaryRayPayloadSetColor(inout RayPayload payload, const float3 color)
{
    payload.m_padding[3] = asuint(color.x);
    payload.m_padding[4] = asuint(color.y);
    payload.m_padding[5] = asuint(color.z);
}

inline float3 secondaryRayPayloadGetWeight(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[7]);
    result.y = asfloat(payload.m_padding[8]);
    result.z = asfloat(payload.m_padding[9]);

    return result;
}

inline void secondaryRayPayloadSetWeight(inout RayPayload payload, const float3 weight)
{
    payload.m_padding[7] = asuint(weight.x);
    payload.m_padding[8] = asuint(weight.y);
    payload.m_padding[9] = asuint(weight.z);
}

/*
struct ShadowRayPayload
{
    uint m_type;
    uint m_nbBounds; // Nombre courrants de rebonds avant le traitement du rayon
    uint m_instanceId;
    float m_maxHitDistance;
    float3 m_weight;
};
*/

inline uint shadowRayPayloadGetNbBounds(in RayPayload payload)
{
    return asuint(payload.m_padding[0]);
}

inline void shadowRayPayloadSetNbBounds(inout RayPayload payload, const uint nbBounds)
{
    payload.m_padding[0] = nbBounds;
}

inline uint shadowRayPayloadGetInstanceId(in RayPayload payload)
{
    return asuint(payload.m_padding[1]);
}

inline void shadowRayPayloadSetInstanceId(inout RayPayload payload, const uint instanceId)
{
    payload.m_padding[1] = instanceId;
}

inline float shadowRayPayloadGetMaxHitDistance(in RayPayload payload)
{
    return asfloat(payload.m_padding[2]);;
}

inline void shadowRayPayloadSetMaxHitDistance(inout RayPayload payload, const float hitDistance)
{
    payload.m_padding[2] = asuint(hitDistance);
}

inline float3 shadowRayPayloadGetWeight(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[4]);
    result.y = asfloat(payload.m_padding[5]);
    result.z = asfloat(payload.m_padding[6]);

    return result;
}

inline void shadowRayPayloadSetWeight(inout RayPayload payload, const float3 weight)
{
    payload.m_padding[4] = asuint(weight.x);
    payload.m_padding[5] = asuint(weight.y);
    payload.m_padding[6] = asuint(weight.z);
}

struct Light
{
    float3 m_radiance;
    float _padding0;

    float3 m_position;
    float _padding1;
};

struct Vertex
{
    float3 m_position;
    float _padding0;

    float3 m_normal;
    float _padding1;

    float2 m_uv;
    float2 _padding2;
};

struct SubMeshData
{
    uint m_materialIndex;
};

struct Material
{
    float3 m_reflectance;
    uint m_albedoTextureIndex;

    float3 m_transmittance;
    uint m_normalTextureIndex;

    float3 m_ks;
    float m_shininess;
};

struct DeviceCamera
{
    float4 m_position;
    float4 m_forward;
    float4 m_up;
    float4 m_right;
    float m_physicalWidth;
    float m_physicalHeight;
    float m_depth;
    float _padding0;
};

struct DeviceConstantInformations
{
    float4 m_position;
    float4 m_forward;
    float4 m_up;
    float4 m_right;
    float m_physicalWidth;
    float m_physicalHeight;
    float m_depth;
    uint m_nbLights;
};

cbuffer Camera : register(b0)
{
    DeviceConstantInformations g_constantInformations;
};
cbuffer HitLocal : register(b1)
{
    SubMeshData g_subMeshData;
};
RWTexture2D<float4> g_output : register(u0);
RaytracingAccelerationStructure g_scene : register(t0);
StructuredBuffer<Vertex> g_vertices : register(t1);
StructuredBuffer<uint3> g_indices : register(t2);
StructuredBuffer<Material> g_materials : register(t3);
StructuredBuffer<uint> g_logicalMaterialIndexToPhysical : register(t4);
StructuredBuffer<Light> g_lights : register(t5);
Texture2D<float4> g_textures[] : register(t6);
SamplerState g_sampler : register(s0);

#endif
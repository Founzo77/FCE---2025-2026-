#ifndef __DATA_H__
#define __DATA_H__

static const uint RAY_TYPE_MAIN = 0;
static const uint RAY_TYPE_SHADOW = 2;
static const uint RAY_TYPE_NEAREST = 3;

static const uint INTEGRATOR_PHONG = 0;
static const uint INTEGRATOR_PATHTRACER = 1;

static const uint INTEGRATOR_DEFAULT = INTEGRATOR_PATHTRACER;
//static const uint INTEGRATOR_DEFAULT = INTEGRATOR_PATHTRACER;

struct PixelAccumulator
{
    float3 m_accumulatedColor;
    uint m_sampleIndex;
    uint m_rngSeed;
    uint2 m_pixelIndex;
    uint2 m_pixelDimension;
};

inline float3 pixelAccumulatorGetFinalColor(in PixelAccumulator pixelAccumulator)
{
    if(pixelAccumulator.m_sampleIndex == 0)
        return float3(0, 0, 0);
    else
        return pixelAccumulator.m_accumulatedColor / pixelAccumulator.m_sampleIndex;
}

struct Intersection
{
    float3 m_worldPosition;
    float3 m_tbnShadingNormal;
    float3 m_worldWi;
    float3x3 m_tbn;
    float3 m_uvw;
};

struct LightAccumulator
{
    float3 m_weight;
    float3 m_accumulatedRadiance;
    uint m_sampleIndex;
};

struct LightSample
{
    uint m_lightIndex;
    float3 m_worldWo;
    float3 m_radiance;
    float3 m_surfaceColor;
};

struct RayPayload
{
    uint m_type;
    uint m_padding[11];
};

/*
struct MainRayPayload
{
    uint m_type; // 

    uint m_nbBounds; // Nombre courrant de rebonds avant le traitement du rayon
    uint m_rngSeed;
    uint _padding0;
    
    float3 m_color; // color output initialise a 0
    float _padding1;

    float3 m_weight; // poids courrant avant le traitement du rayon
    float _padding2;
};
*/

inline uint mainRayPayloadGetNbBounds(in RayPayload payload)
{
    return asuint(payload.m_padding[0]);
}

inline void mainRayPayloadSetNbBounds(inout RayPayload payload, const uint nbBounds)
{
    payload.m_padding[0] = nbBounds;
}

inline uint mainRayPayloadGetRngSeed(in RayPayload payload)
{
    return asuint(payload.m_padding[1]);
}

inline void mainRayPayloadSetRngSeed(inout RayPayload payload, const uint rngSeed)
{
    payload.m_padding[1] = rngSeed;
}

inline float3 mainRayPayloadGetColor(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[3]);
    result.y = asfloat(payload.m_padding[4]);
    result.z = asfloat(payload.m_padding[5]);

    return result;
}

inline void mainRayPayloadSetColor(inout RayPayload payload, const float3 color)
{
    payload.m_padding[3] = asuint(color.x);
    payload.m_padding[4] = asuint(color.y);
    payload.m_padding[5] = asuint(color.z);
}

inline float3 mainRayPayloadGetWeight(in RayPayload payload)
{
    float3 result;
    
    result.x = asfloat(payload.m_padding[7]);
    result.y = asfloat(payload.m_padding[8]);
    result.z = asfloat(payload.m_padding[9]);

    return result;
}

inline void mainRayPayloadSetWeight(inout RayPayload payload, const float3 color)
{
    payload.m_padding[7] = asuint(color.x);
    payload.m_padding[8] = asuint(color.y);
    payload.m_padding[9] = asuint(color.z);
}

/*
struct ShadowRayPayload
{
    uint m_type;
    uint m_nbBounds; // Nombre courrants de rebonds avant le traitement du rayon
    uint m_instanceId;
    float m_maxHitDistance;
    float3 m_weight;
    float _padding0;
    float4 _padding1;
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

/*
struct NearestRayPayload
{
    uint m_type;
    uint m_nbBounds; // Nombre courrants de rebonds avant le traitement du rayon
    uint m_instanceId;
    float m_hitDistance;
    float4 _padding0;
    float4 _padding1;
};
*/

inline uint nearestRayPayloadGetNbBounds(in RayPayload payload)
{
    return asuint(payload.m_padding[0]);
}

inline void nearestRayPayloadSetNbBounds(inout RayPayload payload, const uint nbBounds)
{
    payload.m_padding[0] = nbBounds;
}

inline uint nearestRayPayloadGetInstanceId(in RayPayload payload)
{
    return asuint(payload.m_padding[1]);
}

inline void nearestRayPayloadSetInstanceId(inout RayPayload payload, const uint instanceId)
{
    payload.m_padding[1] = instanceId;
}

inline float nearestRayPayloadGetHitDistance(in RayPayload payload)
{
    return asfloat(payload.m_padding[2]);;
}

inline void nearestRayPayloadSetHitDistance(inout RayPayload payload, const float hitDistance)
{
    payload.m_padding[2] = asuint(hitDistance);
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

struct VolumeData
{
    float3 m_mins;
    float m_scalarMin;

    float3 m_maxs;
    float m_scalarMax;

    uint m_texture3DIndex;
    uint m_transferFunctionTextureIndex;
    float2 _padding2;
};

struct VolumeIntersectionAttributes
{
    float tEntry;
    float tExit;
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

    uint m_backgroundTextureIndex;
    uint m_integratorType;
    uint m_spp;
    uint m_sppIndex;
};

#endif

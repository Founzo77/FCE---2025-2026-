#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "../common/data.hlsli"

static const uint SURFACE_LOBE_DIRAC_REFLECTANCE = 1 << 0;
static const uint SURFACE_LOBE_DIRAC_TRANSMITTANCE = 1 << 1;
static const uint SURFACE_LOBE_DIFFUSE_REFLECTANCE = 1 << 2;
static const uint SURFACE_LOBE_DIFFUSE_TRANSMITTANCE = 1 << 3;

static const uint SURFACE_LOBE_ALL = 0xFFFFFFF;

static const uint SURFACE_LOBE_DIFFUSE = SURFACE_LOBE_DIFFUSE_REFLECTANCE | SURFACE_LOBE_DIFFUSE_TRANSMITTANCE;

inline bool hasType(const uint surfaceLobe, const uint type)
{
    return (surfaceLobe & type) == type;
}

void surfaceGetNormalDeformation(in float2 uv, out float3 normal);

void bsdfEval(in Intersection intersection, in float3 tbnWo, out float3 color, 
    const uint surfaceLobeType);
void bsdfSample(inout RayPayload rayPayload, in Intersection intersection, 
    out float3 tbnWo, out float pdf, const uint surfaceLobeType);
void bsdfPdf(in Intersection intersection, in float3 tbnWo, out float3 pdf,
    const uint surfaceLobeType);

#endif
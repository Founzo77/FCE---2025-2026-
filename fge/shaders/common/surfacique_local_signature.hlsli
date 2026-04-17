#ifndef __SURFACE_LOCAL_SIGNATURE_H__
#define __SURFACE_LOCAL_SIGNATURE_H__

#include "global_signature.hlsli"

cbuffer HitLocalSurface : register(b0, space1)
{
    SubMeshData g_subMeshData;
};

StructuredBuffer<Vertex> g_vertices : register(t0, space1);
StructuredBuffer<uint3>  g_indices  : register(t1, space1);

#endif


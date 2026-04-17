#ifndef __MATHS_H__
#define __MATHS_H__

#include "data.hlsli"

static const float PI = 3.14159265f;

inline float computeRadiance(in float3 color)
{
    return (color.x + color.y + color.z) / 3; 
} 

inline float2 computeUv(in Vertex v0, in Vertex v1, in Vertex v2, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    return v0.m_uv * (1.0 - attr.barycentrics.x - attr.barycentrics.y) +
        v1.m_uv * attr.barycentrics.x +
        v2.m_uv * attr.barycentrics.y;
}

inline float3 computeNormal(in Vertex v0, in Vertex v1, in Vertex v2, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    return normalize(v0.m_normal * (1.0 - attr.barycentrics.x - attr.barycentrics.y) +
        v1.m_normal * attr.barycentrics.x +
        v2.m_normal * attr.barycentrics.y);
}

inline float3 computeHitPosition(in Vertex v0, in Vertex v1, in Vertex v2, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    float b1 = attr.barycentrics.x;
    float b2 = attr.barycentrics.y;
    float b0 = 1.0 - b1 - b2;
    float3 hitPosition = b0 * v0.m_position + b1 * v1.m_position + b2 * v2.m_position;

    return hitPosition;
}

inline float3x3 computeTBN(in Vertex v0, in Vertex v1, in Vertex v2, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    float3 normal = computeNormal(v0, v1, v2, attr);

    float3 dp1 = v1.m_position - v0.m_position;
    float3 dp2 = v2.m_position - v0.m_position;
    float2 duv1 = v1.m_uv - v0.m_uv;
    float2 duv2 = v2.m_uv - v0.m_uv;

    float det = duv1.x * duv2.y - duv1.y * duv2.x;

    float3 tangent, bitangent;

    if(abs(det) < 1e-6)
    {
        // fallback stable
        //tangent = normalize(any(abs(normal.x) > 0.5) 
        //    ? cross(normal, float3(0,1,0)) : cross(normal, float3(1,0,0)));
        //bitangent = normalize(cross(normal, tangent));

        float3 up = abs(normal.z) < 0.999 ? float3(0,0,1) : float3(1,0,0);
        tangent = normalize(cross(up, normal));
        bitangent = cross(normal, tangent);
    }
    else
    {
        float r = 1.0 / det;

        tangent = normalize((dp1 * duv2.y - dp2 * duv1.y) * r);
        tangent = normalize(tangent - normal * dot(normal, tangent));
        bitangent = normalize(cross(normal, tangent));
    }

    float3x3 TBN = float3x3(tangent, bitangent, normal);

    return TBN;
}

inline bool isReflectance(in float3 tbnWi, in float3 tbnWo)
{
    return tbnWi.z * tbnWo.z > 0.0f; // meme cote réflexion
}

inline bool isTransmittance(in float3 tbnWi, in float3 tbnWo)
{
    return tbnWi.z * tbnWo.z < 0.0f; // coté oppose  transmission
}

inline bool isReflectance(in float3 wi, in float3 normal, in float3 wo)
{
    float wiN = dot(wi, normal);
    float woN = dot(wo, normal);
    return wiN * woN > 0.0f;
}

inline bool isTransmittance(in float3 wi, in float3 normal, in float3 wo)
{
    float wiN = dot(wi, normal);
    float woN = dot(wo, normal);
    return wiN * woN < 0.0f;
}

inline bool isPerfectReflection(in float3 wi, in float3 normal, in float3 wo)
{
    float3 expectedWo = reflect(-wi, normal);
    float eps = 1e-4;
    bool isValid = dot(expectedWo, wo) > (1.0 - eps);

    return isValid;
}

inline bool isPerfectRefraction(in float3 wi, in float3 wo)
{
    float eps = 1e-4;
    bool isValid = dot(wo, -wi) > (1.0 - eps);

    return isValid;
}

inline uint hash(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

inline float randomFloat(inout uint seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return (seed * 2.3283064365387e-10); // /2^32
}

inline float randomFloat(inout RayPayload rayPayload)
{
    uint seed = mainRayPayloadGetRngSeed(rayPayload);
    float result = randomFloat(seed);
    mainRayPayloadSetRngSeed(rayPayload, seed);
    return result;
}

inline float2 randomFloat2(inout uint seed)
{
    float x = randomFloat(seed);
    float y = randomFloat(seed);
    return float2(x, y);
}

inline float2 randomFloat2(inout RayPayload rayPayload)
{
    float x = randomFloat(rayPayload);
    float y = randomFloat(rayPayload);
    return float2(x, y);
}

inline float3 randomFloat3(inout uint seed)
{
    float x = randomFloat(seed);
    float y = randomFloat(seed);
    float z = randomFloat(seed);
    return float3(x, y, z);
}

inline float3 randomFloat3(inout RayPayload rayPayload)
{
    float x = randomFloat(rayPayload);
    float y = randomFloat(rayPayload);
    float z = randomFloat(rayPayload);
    return float3(x, y, z);
}

inline uint randomUint(inout uint seed)
{
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    seed = (word >> 22u) ^ word;
    return seed;
}

inline uint randomUint(inout RayPayload rayPayload)
{
    uint seed = mainRayPayloadGetRngSeed(rayPayload);
    uint result = randomUint(seed);
    mainRayPayloadSetRngSeed(rayPayload, seed);
    return result;
}

// TO_DO Verifier plus mettre dans le modelisation
inline float3 cosineSampleHemisphere(float2 u)
{
    float r = sqrt(u.x);
    float theta = 2.0f * PI * u.y;

    float x = r * cos(theta);
    float y = r * sin(theta);
    float z = sqrt(max(0.0f, 1.0f - u.x));

    return float3(x, y, z);
}

inline float3 worldToTbn(float3x3 TBN, float3 v)
{
    return normalize(mul(TBN, v));
}

inline float3 tbnToWorld(float3x3 TBN, float3 v)
{
    return normalize(mul(transpose(TBN), v));
}

float3x3 inverse3x3(float3x3 m)
{
    float3 a = m[0];
    float3 b = m[1];
    float3 c = m[2];

    float3 r0 = cross(b, c);
    float3 r1 = cross(c, a);
    float3 r2 = cross(a, b);

    float det = dot(r2, c);

    // Sécurité (important)
    if (abs(det) < 1e-6)
    {
        return float3x3(1,0,0, 0,1,0, 0,0,1); // fallback
    }

    float invDet = 1.0 / det;

    return float3x3(r0 * invDet, r1 * invDet, r2 * invDet);
}

inline Vertex localToWorld(const Vertex vertex, const float3x4 worldMat3x4)
{
    float3 worldPosition = mul(worldMat3x4, float4(vertex.m_position, 1.0));

    float3 worldNormal = mul((float3x3) worldMat3x4, vertex.m_normal);
    worldNormal = normalize(worldNormal);
    //float3x3 normalMat = transpose(inverse3x3((float3x3)worldMat3x4));
    //float3 worldNormal = normalize(mul(normalMat, vertex.m_normal));

    Vertex result;

    result.m_position = worldPosition;
    result.m_normal = worldNormal;
    result.m_uv = vertex.m_uv;

    return result;
}

#endif
#include "common.hlsli"

#define SPP 10

float RandomFloat(inout uint seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return (seed * 2.3283064365387e-10); // /2^32
}

[shader("raygeneration")]
void RayGenShader()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 launchDim = DispatchRaysDimensions().xy;

    float3 finalColor = 0.0f;

    // Initial seed for RNG (per pixel)
    uint seed = launchIndex.x * 1973 + launchIndex.y * 9277 * 26699;

    for (uint i = 0; i < SPP; i++)
    {
        float2 rnd = float2(RandomFloat(seed), RandomFloat(seed));  // RNG user function
        
        // UV + jitter
        float2 uv  = (float2(launchIndex) + rnd) / float2(launchDim);
        uv.y = 1.0 - uv.y;
        float2 ndc = uv * 2.0f - 1.0f;

        float3 imagePoint =
            g_constantInformations.m_position.xyz +
            g_constantInformations.m_forward.xyz * g_constantInformations.m_depth +
            g_constantInformations.m_right.xyz * ndc.x * 
            g_constantInformations.m_physicalWidth  * 0.5f +
            g_constantInformations.m_up.xyz * ndc.y * g_constantInformations.m_physicalHeight * 0.5f;

        RayDesc ray;
        ray.Origin = g_constantInformations.m_position.xyz;
        ray.Direction = normalize(imagePoint - g_constantInformations.m_position.xyz);
        ray.TMin = 0.001f;
        ray.TMax = 10000.0f;

        RayPayload payload;
        payload.m_type = RAY_TYPE_CAMERA;
        cameraRayPayloadSetNbBounds(payload, 0);
        cameraRayPayloadSetWeight(payload, float3(1,1,1));

        TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, ray, payload);

        finalColor += cameraRayPayloadGetColor(payload);
    }

    finalColor /= SPP;
    g_output[launchIndex] = float4(finalColor, 1.0);
}

#include "common.hlsli"


inline void handleCameraMiss(inout RayPayload payload)
{
    float3 weight = secondaryRayPayloadGetWeight(payload);
    cameraRayPayloadSetColor(payload, float3(0.47, 0.7, 0.99) * weight);
}

inline void handleShadowMiss(inout RayPayload payload)
{
    shadowRayPayloadSetInstanceId(payload, UINT32_MAX);
}

[shader("miss")]
void MissShader(inout RayPayload payload)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_CAMERA:
            handleCameraMiss(payload);
            break;
        case RAY_TYPE_SECONDARY:
            handleCameraMiss(payload);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowMiss(payload);
            break;
        default:
            cameraRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
}

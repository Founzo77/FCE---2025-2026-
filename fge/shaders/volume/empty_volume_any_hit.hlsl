#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

inline void handleCameraRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{

}

inline void handleSecondaryRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{

}

inline void handleShadowRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{

}

inline void handleNearestRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
}

[shader("anyhit")]
void EmptyVolumeAnyHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    IgnoreHit();
    /*
    switch(payload.m_type)
    {
        case RAY_TYPE_CAMERA:
            handleCameraRay(payload, attr);
            break;
        case RAY_TYPE_SECONDARY:
            handleSecondaryRay(payload, attr);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowRay(payload, attr);
            break;
        case RAY_TYPE_NEAREST:
            handleNearestRay(payload, attr);
            break;
        default:
            cameraRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
    */
}

#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

inline void handleCameraRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
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

[shader("closesthit")]
void EmptyVolumeClosestHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_MAIN:
            handleCameraRay(payload, attr);
            break;
        case RAY_TYPE_SHADOW:
            handleShadowRay(payload, attr);
            break;
        case RAY_TYPE_NEAREST:
            handleNearestRay(payload, attr);
            break;
        default:
            mainRayPayloadSetColor(payload, float3(0.5, 0.2, 1));
            break;
    }
}

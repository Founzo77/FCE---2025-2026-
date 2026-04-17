#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

bool IntersectRayAABB(float3 ro, float3 rd,
    float3 bmin, float3 bmax, out float tmin, out float tmax)
{
    float3 invD = 1.0 / rd;
    float3 t0 = (bmin - ro) * invD;
    float3 t1 = (bmax - ro) * invD;

    float3 tmin3 = min(t0, t1);
    float3 tmax3 = max(t0, t1);

    tmin = max(max(tmin3.x, tmin3.y), tmin3.z);
    tmax = min(min(tmax3.x, tmax3.y), tmax3.z);

    return tmax >= max(tmin, 0.0);
}

[shader("intersection")]
void VolumeIntersection()
{
    float3 roWorld = WorldRayOrigin();
    float3 rdWorld = WorldRayDirection();

    float3x4 worldToObject = WorldToObject3x4();

    float3 roObj = mul(worldToObject, float4(roWorld, 1.0));
    float3 rdObj = mul(worldToObject, float4(rdWorld, 0.0));

    float tEntryObj, tExitObj;
    if (!IntersectRayAABB(roObj, rdObj,
                          g_volumeData.m_mins,
                          g_volumeData.m_maxs,
                          tEntryObj, tExitObj))
        return;

    // CAS 1 : rayon part après le volume
    if (tExitObj <= 0.0)
        return;

    // CAS 2 : rayon démarre à l'intérieur
    float tEntryObjClamped = max(tEntryObj, 0.0);

    // Conversion vers world space pour ReportHit
    float3 pObj = roObj + tEntryObjClamped * rdObj;
    float3 pWorld = mul(ObjectToWorld3x4(), float4(pObj, 1.0));
    float tWorld = length(pWorld - roWorld);

    // sécurité DXR
    tWorld = max(tWorld, RayTMin());

    VolumeIntersectionAttributes attr;
    attr.tEntry = tEntryObjClamped;
    attr.tExit = tExitObj;

    ReportHit(tWorld, 0, attr);
}

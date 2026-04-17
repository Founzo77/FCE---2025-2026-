#include "../common/global_signature.hlsli"
#include "../common/volumique_local_signature.hlsli"

// ISOSURFACE CONSTANTS (hard-coded)
static const int ISO_MAX_STEPS = 256;
static const int ISO_REFINE_STEPS = 6;
static const float ISO_VALUE_POS = 0.2;
static const float ISO_VALUE_NEG = -0.2;
static const bool ISO_RENDER_BOTH = true;
static const float ISO_HIT_EPS = 1e-4;
static const float GRAD_UVW_EPS = 0.002;

static const float3 COLOR_POS = float3(1.0, 0.2, 0.2);
static const float3 COLOR_NEG = float3(0.2, 0.3, 1.0);

// HARD-CODED .CUBE GEOMETRY (from your header)
static const int CUBE_NX = 203;
static const int CUBE_NY = 202;
static const int CUBE_NZ = 349;

static const float3 CUBE_ORIGIN = float3(-3.005144, -3.000000, -5.209201);

// Axis step vectors (voxel-to-space)
static const float3 CUBE_VX = float3(0.029894, 0.000000, 0.000000);
static const float3 CUBE_VY = float3(0.000000, 0.029851, 0.000000);
static const float3 CUBE_VZ = float3(0.000000, 0.000000, 0.029939);

// Helpers
inline float sampleField(uint texIndex, float3 uvw)
{
    return g_textures3D[texIndex].SampleLevel(g_sampler, uvw, 0).x;
}

inline bool inside01(float3 v)
{
    return all(v >= 0.0) && all(v <= 1.0);
}

// Convert object-space position -> uvw in [0..1] using cube origin + axis vectors.
// General formula: p = origin + i*vx + j*vy + k*vz.
// We solve for (i,j,k) by projecting onto each axis vector.
inline float3 uvwFromObjPos(float3 posObj)
{
    float3 p = posObj - CUBE_ORIGIN;

    float vx2 = dot(CUBE_VX, CUBE_VX);
    float vy2 = dot(CUBE_VY, CUBE_VY);
    float vz2 = dot(CUBE_VZ, CUBE_VZ);

    // Guard against degenerate axes
    float i = (vx2 > 1e-20) ? dot(p, CUBE_VX) / vx2 : 0.0;
    float j = (vy2 > 1e-20) ? dot(p, CUBE_VY) / vy2 : 0.0;
    float k = (vz2 > 1e-20) ? dot(p, CUBE_VZ) / vz2 : 0.0;

    // Map voxel coordinates [0..N-1] -> uvw [0..1]
    return float3(
        i / (CUBE_NX - 1.0),
        j / (CUBE_NY - 1.0),
        k / (CUBE_NZ - 1.0)
    );
}

inline float3 computeGradientUVW(uint texIndex, float3 uvw)
{
    float h = GRAD_UVW_EPS;

    float fx1 = sampleField(texIndex, uvw + float3(h,0,0));
    float fx0 = sampleField(texIndex, uvw - float3(h,0,0));
    float fy1 = sampleField(texIndex, uvw + float3(0,h,0));
    float fy0 = sampleField(texIndex, uvw - float3(0,h,0));
    float fz1 = sampleField(texIndex, uvw + float3(0,0,h));
    float fz0 = sampleField(texIndex, uvw - float3(0,0,h));

    return float3(fx1 - fx0, fy1 - fy0, fz1 - fz0);
}

// Cherche un crossing de f(t) = iso sur [tEntry, tExit].
// Retourne true si trouvé et donne tHit + uvwHit + fieldHit.
inline bool raymarchIso(
    float3 roObj, float3 rdObj,
    float tEntry, float tExit,
    uint texIndex,
    float iso,
    out float tHit,
    out float3 uvwHit,
    out float fieldAtHit)
{
    float dt = (tExit - tEntry) / (float)ISO_MAX_STEPS;
    float t0 = tEntry;

    // On échantillonne une première fois
    float3 pos0 = roObj + rdObj * t0;
    float3 uvw0 = uvwFromObjPos(pos0);

    // Si on démarre hors volume, on avance jusqu’à rentrer (simple)
    // (tu peux améliorer en intersectant AABB objet, mais tu as déjà tEntry/tExit)
    float f0 = 0.0;
    bool have0 = false;

    if (inside01(uvw0))
    {
        f0 = sampleField(texIndex, uvw0) - iso;
        have0 = true;
    }

    // Marche principale
    [loop]
    for (int i = 0; i < ISO_MAX_STEPS; ++i)
    {
        float t1 = t0 + dt;
        if (t1 > tExit) break;

        float3 pos1 = roObj + rdObj * t1;
        float3 uvw1 = uvwFromObjPos(pos1);

        if (!inside01(uvw1))
        {
            t0 = t1;
            have0 = false;
            continue;
        }

        float f1 = sampleField(texIndex, uvw1) - iso;

        if (!have0)
        {
            // On vient juste de rentrer dans le volume
            f0 = f1;
            uvw0 = uvw1;
            t0 = t1;
            have0 = true;
            continue;
        }

        // Crossing si changement de signe
        if (f0 * f1 < 0.0)
        {
            // Raffinement par dichotomie sur [t0, t1]
            float a = t0;
            float b = t1;
            float fa = f0;
            float fb = f1;

            [unroll]
            for (int r = 0; r < ISO_REFINE_STEPS; ++r)
            {
                float m = 0.5 * (a + b);
                float3 posM = roObj + rdObj * m;
                float3 uvwM = uvwFromObjPos(posM);
                float fm = sampleField(texIndex, uvwM) - iso;

                if (fa * fm < 0.0) { b = m; fb = fm; }
                else               { a = m; fa = fm; }
            }

            tHit = 0.5 * (a + b);
            float3 posHit = roObj + rdObj * tHit;
            uvwHit = uvwFromObjPos(posHit);

            fieldAtHit = sampleField(texIndex, uvwHit);
            return true;
        }

        // Avance
        f0 = f1;
        uvw0 = uvw1;
        t0 = t1;
    }

    return false;
}

// ------------------------------------------------------------
// Isosurface Integrator (remplace DVR)
// ------------------------------------------------------------
inline void resolveIsoIntegrator(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    float3x4 worldToObject = WorldToObject3x4();

    float3 roWorld = WorldRayOrigin();
    float3 rdWorld = WorldRayDirection();

    float3 roObj = mul(worldToObject, float4(roWorld, 1.0));
    float3 rdObj = mul(worldToObject, float4(rdWorld, 0.0));

    float tEntry = max(attr.tEntry, RayTMin());
    float tExit  = attr.tExit;

    // Coupe tExit si une surface opaque est rencontrée dans le volume (comme ton code)
    float t = tEntry;

    RayDesc exitTestRay;
    exitTestRay.Origin = roWorld + rdWorld * t;
    exitTestRay.Direction = rdWorld;
    exitTestRay.TMin = 0.001;
    exitTestRay.TMax = (tExit - t);

    uint nbBounds = secondaryRayPayloadGetNbBounds(payload);

    RayPayload exitTestPayload;
    exitTestPayload.m_type = RAY_TYPE_NEAREST;
    nearestRayPayloadSetNbBounds(exitTestPayload, nbBounds + 1);

    TraceRay(g_scene,
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES,
        0xFF, 0, 1, 0, exitTestRay, exitTestPayload);

    if (nearestRayPayloadGetInstanceId(exitTestPayload) != UINT32_MAX)
    {
        tExit = t + nearestRayPayloadGetHitDistance(exitTestPayload);
    }

    uint texIndex = g_volumeData.m_texture3DIndex;

    // --- Cherche isosurface (+iso) et éventuellement (-iso), on garde la plus proche ---
    bool  hit = false;
    float tHitBest = 0.0;
    float3 uvwBest = 0.0;
    float  fieldBest = 0.0;
    float  isoBest = 0.0;

    float tHitP; float3 uvwP; float fieldP;
    if (raymarchIso(roObj, rdObj, tEntry, tExit, texIndex, ISO_VALUE_POS, tHitP, uvwP, fieldP))
    {
        hit = true;
        tHitBest = tHitP; uvwBest = uvwP; fieldBest = fieldP; isoBest = ISO_VALUE_POS;
    }

    if (ISO_RENDER_BOTH)
    {
        float tHitN; float3 uvwN; float fieldN;
        if (raymarchIso(roObj, rdObj, tEntry, tExit, texIndex, ISO_VALUE_NEG, tHitN, uvwN, fieldN))
        {
            if (!hit || tHitN < tHitBest)
            {
                hit = true;
                tHitBest = tHitN; uvwBest = uvwN; fieldBest = fieldN; isoBest = ISO_VALUE_NEG;
            }
        }
    }

    float3 weight = secondaryRayPayloadGetWeight(payload);

    // --- Si pas de hit iso : on laisse passer (transmittance=1) vers la suite (comme avant) ---
    if (!hit)
    {
        // On continue comme "volume transparent"
        // (pas d’absorption ici : transmittance = 1)
        if (nbBounds < MAX_RAY_NB_BOUNDS)
        {
            RayDesc transmittanceRay;
            transmittanceRay.Origin = roWorld + rdWorld * tExit;
            transmittanceRay.Direction = rdWorld;
            transmittanceRay.TMin = 0.001f;
            transmittanceRay.TMax = 10000.0f;

            RayPayload transmittancePayload;
            transmittancePayload.m_type = RAY_TYPE_SECONDARY;
            secondaryRayPayloadSetNbBounds(transmittancePayload, nbBounds + 1);
            secondaryRayPayloadSetWeight(transmittancePayload, weight);

            TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, transmittanceRay, transmittancePayload);

            cameraRayPayloadSetColor(payload, secondaryRayPayloadGetColor(transmittancePayload));
            return;
        }

        cameraRayPayloadSetColor(payload, float3(0,0,0));
        return;
    }

    // --- Hit iso : shading surface ---
    float3 posObjHit = roObj + rdObj * tHitBest;
    float3 uvwHit = uvwBest;

    // Normal en objet : gradient en UVW (approx). Pour un meilleur résultat, tu peux convertir en espace objet.
    float3 gradUVW = computeGradientUVW(texIndex, uvwHit);

    // Convertit une normale "uvw" en "objet" approximativement via l’échelle du volume
    float3 scaleObj = (g_volumeData.m_maxs - g_volumeData.m_mins);
    float3 nObj = normalize(float3(gradUVW.x / max(scaleObj.x, 1e-6),
                                   gradUVW.y / max(scaleObj.y, 1e-6),
                                   gradUVW.z / max(scaleObj.z, 1e-6)));

    // Normale monde : nWorld = normalize( (worldToObject)^T * nObj )
    float3 nWorld = normalize(mul((float3x3)transpose(worldToObject), nObj));

    float3 posWorldHit = roWorld + rdWorld * tHitBest;

    // Couleur : si tu as une TF, tu peux l’utiliser, sinon rouge/bleu pour +/-.
    float3 baseColor = (isoBest > 0.0) ? COLOR_POS : COLOR_NEG;

    if (g_volumeData.m_transferFunctionTextureIndex != UINT32_MAX)
    {
        // Note : TF souvent définie pour densités >=0.
        // Si ton champ est signé (ψ), tu peux passer abs(fieldBest) ou remapper.
        float tfX = abs(fieldBest); // <- choix simple
        float4 tf = g_textures[g_volumeData.m_transferFunctionTextureIndex]
            .SampleLevel(g_sampler, float2(tfX, 0.5f), 0.0f);
        baseColor = tf.rgb;
    }

    // Surface opaque : on s’arrête ici (pas de transmittance à travers l’isosurface)
    float3 finalColor = baseColor * weight;

    // Si tu veux quand même "voir derrière" (semi-transparence isosurface),
    // tu peux remettre une transmittance < 1 ici.
    cameraRayPayloadSetColor(payload, finalColor);
}

inline void handleCameraRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    resolveIsoIntegrator(payload, attr);
}

inline void handleSecondaryRay(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
    resolveIsoIntegrator(payload, attr);
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
void IsosurfaceClosestHit(inout RayPayload payload, in VolumeIntersectionAttributes attr)
{
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
}

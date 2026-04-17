#include "../common/global_signature.hlsli"
#include "../common/surfacique_local_signature.hlsli"
#include "../common/maths.hlsli"

#include "../integrator/surface_integrator.hlsli"

#include "surface.hlsli"

void surfaceGetNormalDeformation(in float2 uv, out float3 normal)
{
    SubMeshData subMeshData = g_subMeshData;
    Material material = 
        g_materials[g_logicalMaterialIndexToPhysical[subMeshData.m_materialIndex]];

    uint normalTexIndex = material.m_normalTextureIndex;

    float3 normalTex = float3(0, 0, 1);

    if (normalTexIndex != UINT32_MAX) 
    {
        normalTex = g_textures[normalTexIndex].SampleLevel(g_sampler, uv, 0).xyz;
        normalTex = normalize(normalTex * 2.0 - 1.0); // [0,1] -> [-1,1]
    }

    normal = normalTex;
}

static const uint BSDF_TYPE_DIFFUSE = 0; // Pas de miroire et pas de transparence
static const uint BSDF_TYPE_MIRROR = 1; // Miroire
static const uint BSDF_TYPE_GLASS = 2; // Transparent / peut etre un peu miroire
static const uint BSDF_TYPE_METALLIC = 3; // Legerement miroire

static const float WEIGHT_CHOOSE_REFLECTANCE = 0.3;

uint getBsdfType(const Material material)
{
    float reflectance = computeRadiance(material.m_reflectance);
    float transmittance = computeRadiance(material.m_transmittance);

    if (transmittance > 0.1f)
        return BSDF_TYPE_GLASS;

    if (reflectance > 0.5f)
        return BSDF_TYPE_MIRROR;

    if (reflectance > 0.1f)
        return BSDF_TYPE_METALLIC;

    return BSDF_TYPE_DIFFUSE;
}

void bsdfEval(in Intersection intersection, in float3 tbnWo, out float3 color,
    const uint surfaceLobeType)
{
    Material material =
        g_materials[g_logicalMaterialIndexToPhysical[g_subMeshData.m_materialIndex]];

    float2 uv = intersection.m_uvw.xy;

    float3 albedo = float3(1,1,1);

    float cosTheta = max(tbnWo.z, 0.0f);

    if(material.m_albedoTextureIndex != UINT32_MAX)
    {
        uint idx = NonUniformResourceIndex(material.m_albedoTextureIndex);
        albedo = g_textures[idx].SampleLevel(g_sampler, uv, 0).rgb;
    }

    float3 tbnWi = worldToTbn(intersection.m_tbn, intersection.m_worldWi);

    uint type = getBsdfType(material);

    color = float3(0, 0, 0);

    float3 normal = intersection.m_tbnShadingNormal;
    if(tbnWi.z < 0)
        normal = -normal;

    // TO_DO ajouter l'albedo pour les reflectance et autre

    if (type == BSDF_TYPE_DIFFUSE)
    {
        if(isReflectance(tbnWi, tbnWo))
            color = albedo * cosTheta / PI;
    }
    else if (type == BSDF_TYPE_MIRROR)
    {
        if(isPerfectReflection(tbnWi, normal, tbnWo))
            color = material.m_reflectance;
    }
    else if (type == BSDF_TYPE_METALLIC)
    {
        if(isReflectance(tbnWi, tbnWo))
        {
            float metallic = saturate(max(material.m_ks.x, material.m_ks.y));

            float3 diffuse = albedo / PI;
            float3 specular = material.m_ks;

            float3 f_diffuse = diffuse * cosTheta;

            float3 f_specular = float3(0,0,0);

            if(isPerfectReflection(tbnWi, normal, tbnWo))
                f_specular = specular;

            color = (1.0 - metallic) * f_diffuse + metallic * f_specular;
        }
    }
    else // BSDF_TYPE_GLASS
    {
        if(isReflectance(tbnWi, tbnWo))
        {
            if(isPerfectReflection(tbnWi, normal, tbnWo))
                color = material.m_reflectance;
        }
        else
        {
            if(isPerfectRefraction(tbnWi, tbnWo))
            {
                color = material.m_transmittance;
            }
        }
    }
}
void bsdfSample(inout RayPayload rayPayload, in Intersection intersection, 
    out float3 tbnWo, out float pdf, const uint surfaceLobeType)
{
    Material material =
        g_materials[g_logicalMaterialIndexToPhysical[g_subMeshData.m_materialIndex]];

    float3 tbnWi = worldToTbn(intersection.m_tbn, intersection.m_worldWi);
    float3 normal = intersection.m_tbnShadingNormal;
        if(tbnWi.z < 0)
            normal = -normal;

    uint type = getBsdfType(material);

    if (type == BSDF_TYPE_DIFFUSE)
    {
        float2 rand = randomFloat2(rayPayload);
        tbnWo = cosineSampleHemisphere(rand);
        pdf = tbnWo.z / PI;
    }
    else if (type == BSDF_TYPE_MIRROR)
    {
        tbnWo = reflect(-tbnWi, normal);
        pdf = 1;
    }
    else if (type == BSDF_TYPE_METALLIC)
    {
        float metallicProb = saturate(max(material.m_ks.x, material.m_ks.y));

        if (randomFloat(rayPayload) < metallicProb)
        {
            tbnWo = reflect(-tbnWi, normal);
        }
        else
        {
            float2 rand = randomFloat2(rayPayload);
            tbnWo = cosineSampleHemisphere(rand);
        }

        float pdfDiffuse = max(tbnWo.z, 0) / PI;
        float pdfSpecular = isPerfectReflection(tbnWi, normal, tbnWo) ? 1 : 0;

        pdf = metallicProb * pdfSpecular + (1 - metallicProb) * pdfDiffuse;
    }
    else // BSDF_TYPE_GLASS
    {
        float weightChooseReflectance = WEIGHT_CHOOSE_REFLECTANCE;

        if (randomFloat(rayPayload) < weightChooseReflectance)
        {
            tbnWo = reflect(-tbnWi, normal);
            pdf = weightChooseReflectance;
        }
        else
        {
            tbnWo = -tbnWi;
            pdf = 1.0 - weightChooseReflectance;
        }
    }
}
void bsdfPdf(in Intersection intersection, in float3 tbnWo, out float pdf,
    const uint surfaceLobeType)
{
    Material material =
        g_materials[g_logicalMaterialIndexToPhysical[g_subMeshData.m_materialIndex]];

    float3 tbnWi = worldToTbn(intersection.m_tbn, intersection.m_worldWi);

    float3 normal = intersection.m_tbnShadingNormal;
    if(tbnWi.z < 0)
        normal = -normal;

    uint type = getBsdfType(material);

    if (type == BSDF_TYPE_DIFFUSE)
    {
        pdf = max(tbnWo.z, 0.0f) / PI;
    }
    else if (type == BSDF_TYPE_MIRROR)
    {
        pdf = 1.0;
    }
    else if (type == BSDF_TYPE_METALLIC)
    {
        float metallic = saturate(max(material.m_ks.x, material.m_ks.y));

        float pdfDiffuse = max(tbnWo.z, 0) / PI;
        float pdfSpecular = isPerfectReflection(tbnWi, normal, tbnWo) ? 1 : 0;

        pdf = metallic * pdfSpecular + (1 - metallic) * pdfDiffuse;
    }
    else // BSDF_TYPE_GLASS
    {
        float3 tbnWi = worldToTbn(intersection.m_tbn, intersection.m_worldWi);
        float weightChooseReflectance = WEIGHT_CHOOSE_REFLECTANCE;

        if(isReflectance(tbnWi, tbnWo))
        {
            pdf = isPerfectReflection(tbnWi, normal, tbnWo) ? weightChooseReflectance : 0;
        }
        else
        {
            pdf = isPerfectRefraction(tbnWi, tbnWo) ? 1.0 - weightChooseReflectance : 0;
        }
    }
}

inline void handleMainRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    onClosestHitSurface(payload, attr);
}

inline void handleShadowRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    SubMeshData subMeshData = g_subMeshData;
    Material material = g_materials[g_logicalMaterialIndexToPhysical[subMeshData.m_materialIndex]];

    float3 transmittanceWeight = material.m_transmittance;
    uint nbBounds = shadowRayPayloadGetNbBounds(payload);

    if(computeRadiance(transmittanceWeight) >= MIN_TRANSMITTANCE_WEIGHT_FOR_BOUND
        && nbBounds < MAX_RAY_NB_BOUNDS)
    {
        float3 rayDir = WorldRayDirection();
        float tHit = RayTCurrent();
        float tMax = shadowRayPayloadGetMaxHitDistance(payload);

        float3 newOrigin = WorldRayOrigin() + rayDir * (tHit + 1e-4);

        RayDesc rayDesc;
        rayDesc.Origin = newOrigin;
        rayDesc.Direction = rayDir;
        rayDesc.TMin = 0.001f;
        rayDesc.TMax = max(0.0f, tMax - tHit - 1e-4);

        RayPayload newPayload;
        newPayload.m_type = RAY_TYPE_SHADOW;

        float3 newWeight = shadowRayPayloadGetWeight(payload) * transmittanceWeight;
        shadowRayPayloadSetMaxHitDistance(newPayload, rayDesc.TMax);
        shadowRayPayloadSetWeight(newPayload, newWeight);
        shadowRayPayloadSetNbBounds(newPayload, nbBounds + 1);

        TraceRay(g_scene, RAY_FLAG_NONE, 0xFF, 0, 1, 0, rayDesc, newPayload);

        shadowRayPayloadSetWeight(payload, shadowRayPayloadGetWeight(newPayload));
        shadowRayPayloadSetInstanceId(payload, shadowRayPayloadGetInstanceId(newPayload));
    }
    else
    {
        shadowRayPayloadSetInstanceId(payload, InstanceID());
    }
}

inline void handleNearestRay(inout RayPayload payload, 
    in BuiltInTriangleIntersectionAttributes attr)
{
    nearestRayPayloadSetInstanceId(payload, InstanceID());
    nearestRayPayloadSetHitDistance(payload, RayTCurrent());
}

[shader("closesthit")]
void DiffuseClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    switch(payload.m_type)
    {
        case RAY_TYPE_MAIN:
            handleMainRay(payload, attr);
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


#include "../integrator/raygen_integrator.hlsli"

[shader("raygeneration")]
void RayGenShader()
{
    uint2 launchIndex = DispatchRaysIndex().xy;
    uint2 launchDim = DispatchRaysDimensions().xy;

    onRayGen(launchIndex, launchDim);
}

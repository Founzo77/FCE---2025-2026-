#ifndef __PATHTRACER_INTEGRATOR_H__
#define __PATHTRACER_INTEGRATOR_H__

#include "../../common/data.hlsli"

bool pathtracerIterateNextPixelSample(inout PixelAccumulator pixelAccumulator);

bool pathtracerIterateNextPixelSample(inout PixelAccumulator pixelAccumulator)
{
    if(pixelAccumulator.m_sampleIndex < g_constantInformations.m_spp)
    {
        pixelAccumulator.m_sampleIndex++;
        return true;
    }
    else
        return false;
}

#endif

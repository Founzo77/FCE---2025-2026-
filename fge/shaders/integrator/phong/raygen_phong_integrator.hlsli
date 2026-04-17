#ifndef __PHONG_INTEGRATOR_H__
#define __PHONG_INTEGRATOR_H__

#include "../../common/data.hlsli"

bool phongIterateNextPixelSample(inout PixelAccumulator pixelAccumulator);

bool phongIterateNextPixelSample(inout PixelAccumulator pixelAccumulator)
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

#ifndef __VOLUME_LOCAL_SIGNATURE_H__
#define __VOLUME_LOCAL_SIGNATURE_H__

#include "global_signature.hlsli"

cbuffer HitLocalVolume : register(b0, space2)
{
    VolumeData g_volumeData;
};

#endif


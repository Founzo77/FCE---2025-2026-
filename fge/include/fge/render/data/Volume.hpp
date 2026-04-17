#pragma once

#include "VolumeData.hpp"

#include "../memory/DefaultDeviceBuffer.hpp"
#include "../memory/UploadBuffer.hpp"

#include <d3d12.h>

#include <cstdint>

namespace fge
{
    class Volume
    {
    public:
        D3D12_RAYTRACING_GEOMETRY_DESC m_geometryDesc;
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC m_blasDescription;
        DefaultDeviceBuffer m_aabbBuffer;
        UploadBuffer m_aabbUploadBuffer;
        UploadBuffer m_volumeDataUploadBuffer;
        DefaultDeviceBuffer m_blasBuffer;
        DefaultDeviceBuffer m_volumeDataBuffer;
        VolumeData m_volumeData;
        uint32_t m_sbtBaseIndex;
        uint32_t m_hitGroupIndex;

    public:
        Volume() = default;
        ~Volume();

        Volume(const Volume&) = delete;
        Volume(Volume&&) = default;

        Volume& operator=(const Volume&) = delete;
        Volume& operator=(Volume&&) = default;

        void reset();
    };
}
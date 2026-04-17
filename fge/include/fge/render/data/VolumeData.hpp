#pragma once

#include <cstdint>
#include <array>

using std::array;

namespace fge
{
    struct VolumeData
    {
    public:
        array<float, 3> m_maxs;
        float m_scalarMin;
        array<float, 3> m_mins;
        float m_scalarMax;
        uint32_t m_texture3DIndex;
        uint32_t m_transferFunctionTextureIndex;
        array<float, 2> _padding2;

    public:
        VolumeData() = default;
        ~VolumeData() = default;

        VolumeData(const VolumeData&) = default;
        VolumeData(VolumeData&&) = default;

        VolumeData& operator=(const VolumeData&) = default;
        VolumeData& operator=(VolumeData&&) = default;
    };
}
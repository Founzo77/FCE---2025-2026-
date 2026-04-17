#pragma once

#include "Texture3D.hpp"

#include <fge/render/indices.hpp>

#include <anari/anari.h>

#include <memory>
#include <unordered_map>

using fge::LogicalIndex;
using std::shared_ptr;
using std::unordered_map;

namespace fge
{
    class Volume;
    class Texture;
}

namespace fgewa
{
    class Device;
    
    class Volume
    {
    private:
        ANARIVolume m_volume = nullptr;
        ANARISpatialField m_spatialField = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Volume() = default;
        ~Volume();

        Volume(const Volume&) = delete;
        Volume& operator=(const Volume&) = delete;
        Volume(Volume&& other);
        Volume& operator=(Volume&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Volume& volumeData,
            const unordered_map<LogicalIndex, Texture3D>& textures3D,
            const unordered_map<LogicalIndex, fge::Texture>& fgeTextures2D);
        void reset();
        ANARIVolume getVolumeHandle() noexcept;
        const ANARIVolume getVolumeHandle() const noexcept;
    };
}

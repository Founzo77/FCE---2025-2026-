#include <fge/render/data/Volume.hpp>

namespace fge
{
    Volume::~Volume()
    {
        reset();
    }

    void Volume::reset()
    {
        m_geometryDesc = {};
        m_blasDescription = {};

        m_aabbBuffer.reset();
        m_aabbUploadBuffer.reset();
        m_volumeDataUploadBuffer.reset();
        m_blasBuffer.reset();
        m_volumeDataBuffer.reset();

        m_volumeData = {};
        m_sbtBaseIndex = 0;
        m_hitGroupIndex = 0;
    }
}
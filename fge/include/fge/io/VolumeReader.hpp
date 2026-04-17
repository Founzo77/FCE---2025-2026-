#pragma once

#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_unordered_map.h>

#include "../render/data/Volume.hpp"
#include "../render/data/Texture.hpp"

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

using std::string;
using std::array;
using std::unordered_map;
using std::vector;

using oneapi::tbb::concurrent_vector;
using oneapi::tbb::concurrent_unordered_map;

namespace fge
{
    class VolumeReader
    {
    public:
        static void read(const string& pathFile, const string& transfertFunctionPathFile, 
            const uint32_t width, const uint32_t height, const uint32_t depth, 
            const array<float, 3> volumeMin,  const array<float, 3> volumeMax, 
            const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale,
            Volume& volume, Texture& texture,
            concurrent_vector<string>& texturePathFiles, 
            concurrent_unordered_map<string, uint32_t>& pathTextureToTextureIndex);

        static void readRawVolume(const string& pathFile, const string& transfertFunctionPathFile, 
            const uint32_t width, const uint32_t height, const uint32_t depth, 
            const array<float, 3> volumeMin,  const array<float, 3> volumeMax, 
            const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale,
            Volume& volume, Texture& texture);

        static void readRawCube(const string& pathFile, const string& transfertFunctionPathFile, 
            const uint32_t width, const uint32_t height, const uint32_t depth, 
            const array<float, 3> volumeMin,  const array<float, 3> volumeMax, 
            const DXGI_FORMAT format, const bool isLittleEndian, const float scalarScale,
            Volume& volume, Texture& texture);
    };
}
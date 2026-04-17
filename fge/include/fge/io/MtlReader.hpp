#pragma once

#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_unordered_map.h>

#include <fge/render/data/Material.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

using std::vector;
using std::string;
using std::unordered_map;

using oneapi::tbb::concurrent_vector;
using oneapi::tbb::concurrent_unordered_map;

namespace fge
{
    class MtlReader
    {
    public:
        void static read(const string& pathFile, vector<Material>& materials,
            concurrent_vector<string>& texturePathFiles,
            concurrent_unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
            concurrent_unordered_map<string, uint32_t>& pathTextureToTextureIndex,
            unordered_map<string, uint32_t>& nameMaterialToMaterialIndex);
    };
}
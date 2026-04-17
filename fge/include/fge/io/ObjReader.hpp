#pragma once

#include <oneapi/tbb/concurrent_vector.h>
#include <oneapi/tbb/concurrent_unordered_map.h>

#include "../render/data/Mesh.hpp"
#include "../render/data/Material.hpp"

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
    class ObjReader
    {
    public:
        static void read(const string& pathFile, const XMFLOAT3& translation, 
            const XMFLOAT3& rotation, const float scale, 
            Mesh& mesh, vector<Material>& materials, concurrent_vector<string>& texturePathFiles, 
            concurrent_unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
            concurrent_unordered_map<string, uint32_t>& pathTextureToTextureIndex,
            unordered_map<string, uint32_t>& nameMaterialToMaterialIndex);
    };
}
#pragma once

#include "../render/data/Mesh.hpp"
#include "../render/data/Material.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

using std::vector;
using std::string;
using std::unordered_map;

namespace fge
{
    class ObjReader
    {
    public:
        Mesh m_mesh;
        vector<Material> m_materials;
        vector<string> m_texturePathFiles;
        unordered_map<string, uint32_t> m_nameMaterialToTextureIndex;
        unordered_map<string, uint32_t> m_pathTextureToTextureIndex;
        unordered_map<string, uint32_t> m_nameMaterialToMaterialIndex;

    public:
        ObjReader(const string& pathFile, const XMFLOAT3& translation, 
            const XMFLOAT3& rotation, const float scale);

    public:
        static void read(const string& pathFile, const XMFLOAT3& translation, 
            const XMFLOAT3& rotation, const float scale, 
            Mesh& mesh, vector<Material>& materials, vector<string>& texturePathFiles, 
            unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
            unordered_map<string, uint32_t>& pathTextureToTextureIndex,
            unordered_map<string, uint32_t>& nameMaterialToMaterialIndex);
    };
}
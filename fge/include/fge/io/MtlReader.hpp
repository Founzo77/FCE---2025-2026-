#pragma once

#include <fge/render/data/Material.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

using std::vector;
using std::string;
using std::unordered_map;

namespace fge
{
    class MtlReader
    {
    public:
        vector<Material> m_materials;
        vector<string> m_texturePathFiles;
        unordered_map<string, uint32_t> m_nameMaterialToTextureIndex;
        unordered_map<string, uint32_t> m_pathTextureToTextureIndex;
        unordered_map<string, uint32_t> m_nameMaterialToMaterialIndex;

    public:
        MtlReader(const string& pathFile);

    private:

    public:
        void static read(const string& pathFile, vector<Material>& materials,
            vector<string>& texturePathFiles,
            unordered_map<string, uint32_t>& nameMaterialToTextureIndex,
            unordered_map<string, uint32_t>& pathTextureToTextureIndex,
            unordered_map<string, uint32_t>& nameMaterialToMaterialIndex);
    };
}
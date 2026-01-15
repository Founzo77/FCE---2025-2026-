#pragma once

#include "../render/data/Camera.hpp"
#include "../render/data/Mesh.hpp"
#include "../render/data/Instance.hpp"
#include "../render/data/Texture.hpp"
#include "../render/data/Light.hpp"
#include "../render/data/Material.hpp"
#include "../render/data/Texture.hpp"

#include "../render/indices.hpp"

#include <vector>
#include <string>
#include <unordered_map>

using std::unordered_map;
using std::vector;
using std::string;

namespace fge
{
    struct SceneDescription
    {
    public:
        unordered_map<LogicalIndex, Mesh> m_meshes;
        unordered_map<LogicalIndex, Instance> m_instances;
        vector<Light> m_lights;
        vector<Material> m_materials;
        vector<Texture> m_textures;
        Camera m_camera;

        SceneDescription() = default;
        ~SceneDescription() = default;

        SceneDescription(const SceneDescription&) = default;
        SceneDescription(SceneDescription&&) = default;

        SceneDescription& operator=(const SceneDescription&) = default;
        SceneDescription& operator=(SceneDescription&&) = default;
    };
}
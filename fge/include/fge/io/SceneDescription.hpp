#pragma once

#include "../render/data/Background.hpp"
#include "../render/data/Camera.hpp"
#include "../render/data/Mesh.hpp"
#include "../render/data/Volume.hpp"
#include "../render/data/Instance.hpp"
#include "../render/data/Texture.hpp"
#include "../render/data/Light.hpp"
#include "../render/data/Material.hpp"
#include "../render/data/Texture.hpp"

#include "../render/indices.hpp"

#include "../render/RenderConfig.hpp"
#include <fgewa/render/AnariRenderConfig.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

using std::unordered_map;
using std::vector;
using std::string;
using std::variant;

namespace fge
{
    enum class FgeApplicationType
    {
        FGE,
        ANARI
    };

    struct SceneDescription
    {
    public:
        Background m_background;
        unordered_map<LogicalIndex, Mesh> m_meshes;
        unordered_map<LogicalIndex, Volume> m_volumes;
        unordered_map<LogicalIndex, Instance> m_instances;
        vector<Light> m_lights;
        vector<Material> m_materials;
        vector<Texture> m_textures;
        vector<Texture> m_textures3D;
        Camera m_camera;
        FgeApplicationType m_fgeApplicationType;
        variant<RenderConfig, fgewa::AnariRenderConfig> m_renderConfig;

        SceneDescription() = default;
        ~SceneDescription() = default;

        SceneDescription(const SceneDescription&) = default;
        SceneDescription(SceneDescription&&) = default;

        SceneDescription& operator=(const SceneDescription&) = default;
        SceneDescription& operator=(SceneDescription&&) = default;
    };
}
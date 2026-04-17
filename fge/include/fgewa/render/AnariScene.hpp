#pragma once

#include "data/Frame.hpp"
#include "data/Light.hpp"
#include "data/Instance.hpp"
#include "data/Texture2D.hpp"
#include "data/Texture3D.hpp"

#include <fge/render/FgeScene.hpp>
#include <fge/render/indices.hpp>
#include <fge/render/data/Light.hpp>
#include <fge/render/data/Mesh.hpp>
#include <fge/render/data/Volume.hpp>
#include <fge/render/data/Instance.hpp>
#include <fge/render/data/Material.hpp>
#include <fge/render/data/Texture.hpp>

#include <fge/io/SceneDescription.hpp>

#include <vector>
#include <unordered_map>

using fge::LogicalIndex;
using std::unordered_map;
using std::vector;

namespace fgewa
{
    class AnariScene : public fge::FgeScene
    {
    private:
        Frame m_frame;
        vector<Light> m_lights;
        vector<Instance> m_instances;
        unordered_map<LogicalIndex, Texture2D> m_textures2D;
        unordered_map<LogicalIndex, Texture3D> m_textures3D;
        unordered_map<LogicalIndex, fge::Light> m_fgeLights;
        unordered_map<LogicalIndex, fge::Mesh> m_fgeMeshes;
        unordered_map<LogicalIndex, fge::Volume> m_fgeVolumes;
        unordered_map<LogicalIndex, fge::Instance> m_fgeInstances;
        unordered_map<LogicalIndex, fge::Material> m_fgeMaterials;
        unordered_map<LogicalIndex, fge::Texture> m_fgeTextures2D;
        unordered_map<LogicalIndex, fge::Texture> m_fgeTextures3D;
        shared_ptr<Device> m_device;

    public:
        AnariScene() = default;
        virtual ~AnariScene();

        const fge::Light& getLight(const LogicalIndex lightIndex) override;
        void addLight(fge::Light& newLight, const LogicalIndex lightIndex) override;
        void modifyLight(fge::Light& modifiedLight, const LogicalIndex lightIndex) override;
        void deleteLight(const LogicalIndex lightIndex) override;
        
        const fge::Instance& getInstance(const LogicalIndex instanceIndex) override;
        void addInstance(fge::Instance& newInstance, const LogicalIndex instanceIndex) override;
        void modifyInstance(fge::Instance& newInstance, const LogicalIndex instanceIndex) override;
        void deleteInstance(const LogicalIndex instanceIndex) override;

        void moveCameraForward(float distance) override;
        void moveCameraRight(float distance) override;
        void moveCameraUp(float distance) override;
        void rotateCameraY(float distance) override;
        void rotateCamera(float x, float y) override;

        void setCamera(const fge::Camera& camera) override;

        uint32_t getNbMaxMeshes() override;
        uint32_t getNbMaxSubMeshes() override;
        uint32_t getNbMaxInstances() override;
        uint32_t getNbMaxMaterials() override;
        uint32_t getNbMaxTextures() override;
        uint32_t getNbMaxLights() override;

        void buildScene(shared_ptr<Device> device, fge::SceneDescription& sceneDescription,
            const uint32_t width, const uint32_t height);

        Frame& getFrame() noexcept;
        const Frame& getFrame() const noexcept;
    };
}

#include <fgewa/render/AnariScene.hpp>
#include <fgewa/render/data/ImplementationCapability.hpp>

#include <fge/io/GlobalLogger.hpp>

using fge::globalLogger;

namespace fgewa
{
    AnariScene::~AnariScene()
    {
        m_textures3D.clear();
        m_textures2D.clear();
        m_instances.clear();
        m_lights.clear();
        m_frame.reset();
        m_device.reset();
    }

    const fge::Light& AnariScene::getLight(const LogicalIndex lightIndex)
    {
        return m_fgeLights.at(lightIndex.m_index);
    }

    void AnariScene::addLight(fge::Light& newLight, const LogicalIndex lightIndex)
    {

    }

    void AnariScene::modifyLight(fge::Light& modifiedLight, const LogicalIndex lightIndex)
    {

    }

    void AnariScene::deleteLight(const LogicalIndex lightIndex)
    {

    }

    const fge::Instance& AnariScene::getInstance(const LogicalIndex instanceIndex)
    {
        return m_fgeInstances.at(instanceIndex.m_index);
    }

    void AnariScene::addInstance(fge::Instance& newInstance, const LogicalIndex instanceIndex)
    {

    }

    void AnariScene::modifyInstance(fge::Instance& newInstance, const LogicalIndex instanceIndex)
    {
        if(m_fgeInstances.find(instanceIndex) != m_fgeInstances.end())
        {
            m_fgeInstances.at(instanceIndex) = newInstance;
            // TO_DO Faire avec les groupes quand stockes
            m_instances.at(instanceIndex.m_index).setTransformMatrix(newInstance.m_transform);
        }
    }

    void AnariScene::deleteInstance(const LogicalIndex instanceIndex)
    {

    }

    void AnariScene::moveCameraForward(float distance)
    {
        fge::Camera camera = m_frame.getCamera().getCameraData();
        camera.moveForward(distance);
        m_frame.getCamera().setCameraData(camera);
    }

    void AnariScene::moveCameraRight(float distance)
    {
        fge::Camera camera = m_frame.getCamera().getCameraData();
        camera.moveRight(distance);
        m_frame.getCamera().setCameraData(camera);
    }

    void AnariScene::moveCameraUp(float distance)
    {
        fge::Camera camera = m_frame.getCamera().getCameraData();
        camera.moveUp(distance);
        m_frame.getCamera().setCameraData(camera);
    }

    void AnariScene::rotateCameraY(float distance)
    {
        fge::Camera camera = m_frame.getCamera().getCameraData();
        camera.rotateY(distance);
        m_frame.getCamera().setCameraData(camera);
    }

    void AnariScene::rotateCamera(float x, float y)
    {
        fge::Camera camera = m_frame.getCamera().getCameraData();
        camera.rotate(x, y);
        m_frame.getCamera().setCameraData(camera);
    }

    void AnariScene::setCamera(const fge::Camera& camera)
    {
        m_frame.getCamera().setCameraData(camera);
    }

    uint32_t AnariScene::getNbMaxMeshes()
    {
        return 1000;
    }

    uint32_t AnariScene::getNbMaxSubMeshes()
    {
        return 1000;
    }

    uint32_t AnariScene::getNbMaxInstances()
    {
        return 1000;
    }

    uint32_t AnariScene::getNbMaxMaterials()
    {
        return 1000;
    }

    uint32_t AnariScene::getNbMaxTextures()
    {
        return 1000;
    }

    uint32_t AnariScene::getNbMaxLights()
    {
        return 1000;
    }

    void AnariScene::buildScene(shared_ptr<Device> device, fge::SceneDescription& sceneDescription,
        const uint32_t width, const uint32_t height)
    {
        // Gerer les stockeur fge

        m_device = device;

        Camera camera;
        camera.initialize(m_device, sceneDescription.m_camera);

        m_lights.clear();
        m_fgeLights.clear();

        for (size_t i = 0; i < sceneDescription.m_lights.size(); i++)
        {
            Light light;
            light.initialize(m_device, sceneDescription.m_lights[i]);
            m_lights.push_back(std::move(light));

            m_fgeLights[LogicalIndex{static_cast<uint32_t>(i)}] = sceneDescription.m_lights[i];
        }

        m_textures2D.clear();
        m_fgeTextures2D.clear();

        for(size_t i = 0; i < sceneDescription.m_textures.size(); i++)
        {
            Texture2D texture2D;
            texture2D.initialize(m_device, sceneDescription.m_textures[i]);
            m_textures2D.insert({LogicalIndex{static_cast<uint32_t>(i)}, std::move(texture2D)});
            m_fgeTextures2D.insert({LogicalIndex{static_cast<uint32_t>(i)}, 
                std::move(sceneDescription.m_textures[i])});
        }

        // Background : HDRI light
        /* // TO_DO Peux creer des crash dans les implementations donc pas implementer pour l'instant
        if (sceneDescription.m_background.m_textureIndex != UINT32_MAX)
        {
            const uint32_t textureIndex = sceneDescription.m_background.m_textureIndex;
            auto itTexture2D = m_textures2D.find(LogicalIndex{textureIndex});

            if (itTexture2D != m_textures2D.end())
            {
                Light envLight;
                envLight.initializeHdri(m_device, itTexture2D->second);
                m_lights.push_back(std::move(envLight));
            }
        }
        */

        m_textures3D.clear();
        m_fgeTextures3D.clear();

        for(size_t i = 0; i < sceneDescription.m_textures3D.size(); i++)
        {
            Texture3D texture3D;
            texture3D.initialize(m_device, sceneDescription.m_textures3D[i]);
            m_textures3D.insert({LogicalIndex{static_cast<uint32_t>(i)}, std::move(texture3D)});
            m_fgeTextures3D.insert({LogicalIndex{static_cast<uint32_t>(i)}, 
                std::move(sceneDescription.m_textures3D[i])});
        }

        // Instances ((fgeMesh -> geometry -> surface) | (fgeVolume -> spatialField -> volume) -> group -> instance)
        m_instances.clear();
        m_fgeInstances.clear();
        m_fgeMeshes.clear();
        m_fgeVolumes.clear();
        m_fgeMaterials.clear();

        for (auto& [instanceIndex, instanceData] : sceneDescription.m_instances)
        {
            // TO_DO Stocker les groupes
            //if(instanceData.m_geometryReference.m_type == fge::GeometryType::AABB)
            //    continue;
                
            Group group;

            if(instanceData.m_geometryReference.m_type == fge::GeometryType::AABB)
            {
                const fge::Volume& volumeData = 
                    sceneDescription.m_volumes.at(instanceData.m_geometryReference.m_geometryIndex);

                vector<Volume> volumes;

                Volume volume;
                volume.initialize(m_device, volumeData, m_textures3D, m_fgeTextures2D);

                volumes.push_back(std::move(volume));

                group.initialize(m_device, std::move(volumes));
            }
            else if(instanceData.m_geometryReference.m_type == fge::GeometryType::TRIANGLES)
            {
                // Mesh
                const fge::Mesh& mesh = 
                    sceneDescription.m_meshes.at(instanceData.m_geometryReference.m_geometryIndex);

                vector<Surface> surfaces;

                for(uint32_t subMeshId = 0; subMeshId < mesh.m_subMeshes.size(); subMeshId++)
                {
                    const fge::SubMesh& subMesh = mesh.m_subMeshes[subMeshId];
                    Geometry geometry;
                    // TO_DO ne pas recrer la geometry
                    geometry.initialize(m_device, mesh, subMeshId); // full mesh (duplication)

                    const fge::Material& materialData = 
                        sceneDescription.m_materials.at(subMesh.m_materialIndex);

                    Material material;
                    material.initialize(m_device, materialData, m_textures2D);

                    Surface surface;
                    surface.initialize(m_device, std::move(geometry), std::move(material));

                    surfaces.push_back(std::move(surface));
                }

                group.initialize(m_device, std::move(surfaces));
            }

            Instance instance;
            instance.initialize(m_device, std::move(group), instanceData);

            m_instances.push_back(std::move(instance));
            m_fgeInstances[instanceIndex] = instanceData;
        }

        World world;
        world.initialize(m_device, m_instances, m_lights);

        Renderer renderer;
        AnariRenderConfig& anariConfig = std::get<AnariRenderConfig>(sceneDescription.m_renderConfig);
        renderer.initialize(m_device, anariConfig);

        m_frame.reset();
        ImplementationCapability capability = 
            ImplementationCapability::fromLibraryName(anariConfig.m_libraryName);
        m_frame.initialize(m_device, capability,
            std::move(camera), std::move(world), std::move(renderer), width, height);
    }

    Frame& AnariScene::getFrame() noexcept
    {
        return m_frame;
    }

    const Frame& AnariScene::getFrame() const noexcept
    {
        return m_frame;
    }
}
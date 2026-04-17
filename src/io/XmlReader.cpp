#include <fce/io/XmlReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fce/objects/GameObject.hpp>

#include <fge/render/data/GeometryReference.hpp>

#include <algorithm>

using fge::globalLogger;
using fge::Instance;
using fge::Light;
using fge::Transform;
using fge::Basis;
using fge::GeometryReference;
using fge::GeometryType;

namespace fce
{
    XmlReader::XmlReader(const string& pathFile)
    {
        // TO_DO Faire log et verification de tous

        globalLogger().debug() << "Loading scene from file: " << pathFile;

        tinyxml2::XMLDocument document;

        if (document.LoadFile(pathFile.c_str()) != tinyxml2::XML_SUCCESS) 
        {
            globalLogger().error() << "Failed to load XML file: " << pathFile;
            globalLogger().error() << "TinyXML2 error: " << document.ErrorStr();
            exit(EXIT_FAILURE);
        }

        tinyxml2::XMLElement* root = document.FirstChildElement("scene");

        if(!root)
        {
            globalLogger().error() << "Root element <scene> not found!";
            exit(EXIT_FAILURE);
        }

        loadRenderReader(*root);
        loadGameObjects(*root);

        globalLogger().debug() << "Scene successfully loaded: " << pathFile;
    }

    void XmlReader::loadRenderReader(tinyxml2::XMLElement& root)
    {
        m_renderReader = std::make_unique<fge::XmlReader>(root);
    }

    void XmlReader::loadGameObjects(tinyxml2::XMLElement& root)
    {
        tinyxml2::XMLElement* gameObjectsElement = root.FirstChildElement("game_objects");

        uint32_t instanceId = 0;

        if(gameObjectsElement)
        {
            int renderCameraId = -1;
            gameObjectsElement->QueryIntAttribute("render_camera_id", &renderCameraId);
            m_renderCameraGameObject = LogicalIndex(renderCameraId);

            for (tinyxml2::XMLElement* gameObject = 
                gameObjectsElement->FirstChildElement("game_object"); 
                gameObject != nullptr; 
                gameObject = gameObject->NextSiblingElement("game_object")) 
            {
                int id = -1;
                gameObject->QueryIntAttribute("id", &id);

                unordered_map<EngineComponentType, vector<unique_ptr<EngineComponent>>> components;
                unordered_map<ScriptComponentType, vector<unique_ptr<ScriptComponent>>> scripts;

                TransformComponent transformComponent;

                tinyxml2::XMLElement* transformElement = gameObject->FirstChildElement("transform");
                if(transformElement)
                {
                    transformComponent = loadTransformComponent(*transformElement);
                    
                    vector<std::unique_ptr<EngineComponent>> list;
                    list.push_back(std::make_unique<TransformComponent>(transformComponent));
                    components.insert({ EngineComponentType::TRANSFORM, std::move(list) });
                }

                tinyxml2::XMLElement* renderElement = gameObject->FirstChildElement("render");
                if(renderElement)
                {
                    RenderComponent renderComponent = loadRenderComponent(*renderElement);
                    renderComponent.m_instanceIndex = instanceId;

                    Instance instance;
                    instance.m_geometryReference = renderComponent.m_geometryReference;
                    instance.m_transform = transformComponent.getTransform().worldMatrix();

                    m_renderReader->m_sceneDescription.m_instances.insert(
                        { LogicalIndex(instanceId), instance });
                    instanceId++;

                    vector<std::unique_ptr<EngineComponent>> list;
                    list.push_back(std::make_unique<RenderComponent>(renderComponent));
                    components.insert({ EngineComponentType::RENDER, std::move(list) });
                }

                tinyxml2::XMLElement* lightPointElement = 
                    gameObject->FirstChildElement("light_point");
                if(lightPointElement)
                {
                    LightPointComponent lightPointComponent = 
                        loadLightPointComponent(*lightPointElement);

                    XMFLOAT3 radiance = lightPointComponent.m_radiance;
                    XMFLOAT3 position = transformComponent.getTransform().m_position;

                    m_renderReader->m_sceneDescription.m_lights.push_back(Light(radiance, position));

                    vector<std::unique_ptr<EngineComponent>> list;
                    list.push_back(std::make_unique<LightPointComponent>(lightPointComponent));
                    components.insert({ EngineComponentType::LIGHT_POINT, std::move(list) });
                }

                tinyxml2::XMLElement* cameraElement = gameObject->FirstChildElement("camera");
                if(cameraElement)
                {
                    CameraComponent cameraComponent = loadCameraComponent(*cameraElement);
                    vector<std::unique_ptr<EngineComponent>> list;
                    list.push_back(std::make_unique<CameraComponent>(cameraComponent));
                    components.insert({ EngineComponentType::CAMERA, std::move(list) });
                }

                tinyxml2::XMLElement* playerElement = gameObject->FirstChildElement("player");
                if(playerElement)
                {
                    PlayerComponent playerComponent = 
                        loadPlayerComponent(*playerElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<PlayerComponent>(playerComponent));
                    scripts.insert({ ScriptComponentType::PLAYER, std::move(list) });
                }

                tinyxml2::XMLElement* thirdViewPlayerElement = 
                    gameObject->FirstChildElement("third_view_player");
                if(thirdViewPlayerElement)
                {
                    ThirdViewPlayerComponent thirdViewPlayerComponent = 
                        loadThirdViewPlayerComponent(*thirdViewPlayerElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<ThirdViewPlayerComponent>(thirdViewPlayerComponent));
                    scripts.insert({ ScriptComponentType::THIRD_VIEW_PLAYER, std::move(list) });
                }

                tinyxml2::XMLElement* zombieElement = gameObject->FirstChildElement("zombie");
                if(zombieElement)
                {
                    ZombieComponent zombieComponent = loadZombieComponent(*zombieElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<ZombieComponent>(zombieComponent));
                    scripts.insert({ ScriptComponentType::ZOMBIE, std::move(list) });
                }

                tinyxml2::XMLElement* cameraManagerElement = 
                    gameObject->FirstChildElement("camera_manager");
                if(cameraManagerElement)
                {
                    CameraManagerComponent cameraManagerComponent = 
                        loadCameraManagerComponent(*cameraManagerElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<CameraManagerComponent>(cameraManagerComponent));
                    scripts.insert({ ScriptComponentType::CAMERA_MANAGER, std::move(list) });
                }

                tinyxml2::XMLElement* playerManagerElement = 
                    gameObject->FirstChildElement("player_manager");
                if(playerManagerElement)
                {
                    PlayerManagerComponent playerManagerComponent = 
                        loadPlayerManagerComponent(*playerManagerElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<PlayerManagerComponent>(playerManagerComponent));
                    scripts.insert({ ScriptComponentType::PLAYER_MANAGER, std::move(list) });
                }

                tinyxml2::XMLElement* bulletElement = 
                    gameObject->FirstChildElement("bullet");
                if(bulletElement)
                {
                    BulletComponent bulletComponent = 
                        loadBulletComponent(*bulletElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<BulletComponent>(bulletComponent));
                    scripts.insert({ ScriptComponentType::BULLET, std::move(list) });
                }

                tinyxml2::XMLElement* shooterElement = 
                    gameObject->FirstChildElement("shooter");
                if(shooterElement)
                {
                    ShooterComponent shooterComponent = 
                        loadShooterComponent(*shooterElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<ShooterComponent>(shooterComponent));
                    scripts.insert({ ScriptComponentType::SHOOTER, std::move(list) });
                }

                tinyxml2::XMLElement* particleElement = 
                    gameObject->FirstChildElement("particle");
                if(particleElement)
                {
                    ParticleComponent particleComponent = loadParticleComponent(*particleElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<ParticleComponent>(particleComponent));
                    scripts.insert({ ScriptComponentType::PARTICLE, std::move(list) });
                }

                tinyxml2::XMLElement* particleGeneratorElement = 
                    gameObject->FirstChildElement("particle_generator");
                if(particleGeneratorElement)
                {
                    ParticleGeneratorComponent particleGeneratorComponent = 
                        loadParticleGeneratorComponent(*particleGeneratorElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<ParticleGeneratorComponent>(particleGeneratorComponent));
                    scripts.insert({ ScriptComponentType::PARTICLE_GENERATOR, std::move(list) });
                }

                tinyxml2::XMLElement* debugPositionElement = 
                    gameObject->FirstChildElement("debug_position");
                if(debugPositionElement)
                {
                    DebugPositionComponent debugPositionComponent = 
                        loadDebugPositionComponent(*debugPositionElement);
                    vector<std::unique_ptr<ScriptComponent>> list;
                    list.push_back(
                        std::make_unique<DebugPositionComponent>(debugPositionComponent));
                    scripts.insert({ ScriptComponentType::DEBUG_POSITION, std::move(list) });
                }

                m_gameObjects.push_back({ 
                    LogicalIndex(id), std::move(components), std::move(scripts) });
            }

            std::sort(
                m_gameObjects.begin(),
                m_gameObjects.end(),
                [](const GameObject& a, const GameObject& b)
                {
                    return a.getIndex().m_index < b.getIndex().m_index;
                });
        }
        else
        {
            globalLogger().error() << "No <game_objects> element found!";
            exit(EXIT_FAILURE);
        }
    }

    TransformComponent XmlReader::loadTransformComponent(tinyxml2::XMLElement& root)
    {
        XMFLOAT3 translation = fge::XmlReader::loadVector3D(root, string("translation"));
        XMFLOAT3 rotation = fge::XmlReader::loadVector3D(root, string("rotation"));
        XMFLOAT3 scale = fge::XmlReader::loadVector3D(root, string("scale"));

        Transform transform;

        transform.setPosition(translation);

        XMVECTOR rotRad = XMVectorSet(
            XMConvertToRadians(rotation.x),
            XMConvertToRadians(rotation.y),
            XMConvertToRadians(rotation.z),
            0.f);

        XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(rotRad);

        transform.m_basis = Basis(rotMatrix);
        transform.m_basis.scale(scale);

        return TransformComponent(transform);
    }

    CameraComponent XmlReader::loadCameraComponent(tinyxml2::XMLElement& root)
    {
        float physicalWidth = 0;
        float physicalHeight = 0;
        float depth = 0;

        root.QueryFloatAttribute("width", &physicalWidth);
        root.QueryFloatAttribute("height", &physicalHeight);
        root.QueryFloatAttribute("depth", &depth);

        return CameraComponent(physicalWidth, physicalHeight, depth);
    }

    RenderComponent XmlReader::loadRenderComponent(tinyxml2::XMLElement& root)
    {
        int geometry_id = 0;
        root.QueryIntAttribute("geometry_id", &geometry_id);
        int id = 0;
        root.QueryIntAttribute("id", &id);

        const char* typeStr = root.Attribute("type");
        if (!typeStr)
        {
            globalLogger().error() << "Instance has no type attribute!";
            exit(EXIT_FAILURE);
        }

        GeometryType geometryType;

        if (strcmp(typeStr, "surface") == 0)
        {
            geometryType = GeometryType::TRIANGLES;
        }
        else if (strcmp(typeStr, "volume") == 0)
        {
            geometryType = GeometryType::AABB;
        }
        else
        {
            globalLogger().error() << "Unknown instance type: " << typeStr;
            exit(EXIT_FAILURE);
        }

        RenderComponent renderComponent;
        renderComponent.m_geometryReference.m_geometryIndex = geometry_id;
        renderComponent.m_geometryReference.m_type = geometryType;

        return renderComponent;
    }

    ThirdViewPlayerComponent XmlReader::loadThirdViewPlayerComponent(tinyxml2::XMLElement& root)
    {
        int cameraId = 0;
        int meshId = 0;
        root.QueryIntAttribute("camera_game_object_id", &cameraId);
        root.QueryIntAttribute("mesh_game_object_id", &meshId);

        float mouseSensitivity = 0;
        root.QueryFloatAttribute("mouse_sensitivity", &mouseSensitivity);
        float minDistance = 0;
        root.QueryFloatAttribute("min_distance", &minDistance);
        float maxDistance = 0;
        root.QueryFloatAttribute("max_distance", &maxDistance);
        float heightOffset = 0;
        root.QueryFloatAttribute("height_offset", &heightOffset);
        float sideOffset = 0;
        root.QueryFloatAttribute("side_offset", &sideOffset);

        return ThirdViewPlayerComponent(meshId, cameraId, mouseSensitivity,
            minDistance, maxDistance, heightOffset, sideOffset);
    }

    PlayerComponent XmlReader::loadPlayerComponent(tinyxml2::XMLElement& root)
    {
        float moveSpeed = 0;
        root.QueryFloatAttribute("move_speed", &moveSpeed);
        float rotateSpeed = 0;
        root.QueryFloatAttribute("rotate_speed", &rotateSpeed);
        float acceleration = 0;
        root.QueryFloatAttribute("acceleration", &acceleration);

        return PlayerComponent(moveSpeed, rotateSpeed, acceleration);
    }

    LightPointComponent XmlReader::loadLightPointComponent(tinyxml2::XMLElement& root)
    {
        int id = -1;
        root.QueryIntAttribute("id", &id);
        XMFLOAT3 radiance = fge::XmlReader::loadVector3D(root, string("radiance"));

        return LightPointComponent({ static_cast<uint32_t>(id) }, radiance);
    }

    ZombieComponent XmlReader::loadZombieComponent(tinyxml2::XMLElement& root)
    {
        int id = -1;
        root.QueryIntAttribute("target_game_object_id", &id);
        float moveSpeed = 0;
        root.QueryFloatAttribute("move_speed", &moveSpeed);
        float rotateSpeed = 0;
        root.QueryFloatAttribute("rotate_speed", &rotateSpeed);
        float stopDistance = 0;
        root.QueryFloatAttribute("stop_distance", &stopDistance);

        return ZombieComponent(LogicalIndex(static_cast<uint32_t>(id)), moveSpeed,
            rotateSpeed, stopDistance);
    }

    CameraManagerComponent XmlReader::loadCameraManagerComponent(tinyxml2::XMLElement& root)
    {
        vector<LogicalIndex> ids;

        const char* attr = root.Attribute("camera_game_object_ids");

        std::stringstream ss(attr);
        int value = 0;

        while (ss >> value)
        {
            ids.emplace_back(LogicalIndex(value));
        }

        return CameraManagerComponent(ids);
    }

    PlayerManagerComponent XmlReader::loadPlayerManagerComponent(tinyxml2::XMLElement& root)
    {
        vector<LogicalIndex> ids;

        const char* attr = root.Attribute("player_game_object_ids");

        std::stringstream ss(attr);
        int value = 0;

        while (ss >> value)
        {
            ids.emplace_back(LogicalIndex(value));
        }

        float moveSpeed = 0;
        root.QueryFloatAttribute("move_speed", &moveSpeed);
        float rotateSpeed = 0;
        root.QueryFloatAttribute("rotate_speed", &rotateSpeed);
        float acceleration = 0;
        root.QueryFloatAttribute("acceleration", &acceleration);

        return PlayerManagerComponent(ids, moveSpeed, rotateSpeed, acceleration);
    }

    BulletComponent fce::XmlReader::loadBulletComponent(tinyxml2::XMLElement& root)
    {
        float bulletSpeed = 0;
        root.QueryFloatAttribute("bullet_speed", &bulletSpeed);
        double lifeTime = 0;
        root.QueryDoubleAttribute("life_time", &lifeTime);

        return BulletComponent(bulletSpeed, lifeTime);
    }

    ShooterComponent fce::XmlReader::loadShooterComponent(tinyxml2::XMLElement &root)
    {
        int id = -1;
        root.QueryIntAttribute("bullet_mesh_id", &id);
        // TO_DO Gerer le nullptr
        tinyxml2::XMLElement* bulletElement = root.FirstChildElement("bullet");
        BulletComponent bulletComponent = loadBulletComponent(*bulletElement);

        return ShooterComponent(LogicalIndex(static_cast<uint32_t>(id)), bulletComponent);
    }

    ParticleComponent XmlReader::loadParticleComponent(tinyxml2::XMLElement& root)
    {
        float particleSpeed = 0;
        root.QueryFloatAttribute("particle_speed", &particleSpeed);
        double lifeTime = 0;
        root.QueryDoubleAttribute("life_time", &lifeTime);

        return ParticleComponent(particleSpeed, lifeTime);
    }

    ParticleGeneratorComponent XmlReader::loadParticleGeneratorComponent(tinyxml2::XMLElement &root)
    {
        int particleTargetId = -1;
        root.QueryIntAttribute("particle_volume_id", &particleTargetId);
        double generateFrequency = 0;
        root.QueryDoubleAttribute("generate_frequency", &generateFrequency);
        // TO_DO Gerer le nullptr
        tinyxml2::XMLElement* particleElement = root.FirstChildElement("particle");
        ParticleComponent particleComponent = loadParticleComponent(*particleElement);

        return ParticleGeneratorComponent(LogicalIndex(static_cast<uint32_t>(particleTargetId)), 
            particleComponent, generateFrequency);
    }

    DebugPositionComponent XmlReader::loadDebugPositionComponent(tinyxml2::XMLElement& root)
    {
        return DebugPositionComponent();
    }
}

#include <fce/io/XmlReader.hpp>
#include <fge/io/GlobalLogger.hpp>

#include <fce/objects/GameObject.hpp>

#include <algorithm>

using fge::globalLogger;
using fge::Instance;
using fge::Light;
using fge::Transform;
using fge::Basis;

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
                    instance.m_meshIndex = renderComponent.m_meshIndex.m_index;
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
        int mesh_id = 0;
        root.QueryIntAttribute("mesh_id", &mesh_id);

        RenderComponent renderComponent;
        renderComponent.m_meshIndex = mesh_id;

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

    DebugPositionComponent XmlReader::loadDebugPositionComponent(tinyxml2::XMLElement& root)
    {
        return DebugPositionComponent();
    }
}

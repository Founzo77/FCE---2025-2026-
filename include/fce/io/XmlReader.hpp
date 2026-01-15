#pragma once

#include "../objects/GameObject.hpp"

#include "../objects/engine_components/TransformComponent.hpp"
#include "../objects/engine_components/CameraComponent.hpp"
#include "../objects/engine_components/RenderComponent.hpp"
#include "../objects/engine_components/LightPointComponent.hpp"

#include "../objects/scripts/ThirdViewPlayerComponent.hpp"
#include "../objects/scripts/PlayerComponent.hpp"
#include "../objects/scripts/ZombieComponent.hpp"
#include "../objects/scripts/CameraManagerComponent.hpp"
#include "../objects/scripts/PlayerManagerComponent.hpp"
#include "../objects/scripts/DebugPositionComponent.hpp"

#include <fge/io/XmlReader.hpp>
#include <fge/render/indices.hpp>

#include <memory>
#include <vector>
#include <string>

using fge::LogicalIndex;

using std::unique_ptr;
using std::vector;
using std::string;

namespace fce
{
    class XmlReader
    {
    public:
        unique_ptr<fge::XmlReader> m_renderReader;
        vector<GameObject> m_gameObjects;
        LogicalIndex m_renderCameraGameObject;

    public:
        XmlReader(const string& pathFile);

    private:
        void loadRenderReader(tinyxml2::XMLElement& root);
        void loadGameObjects(tinyxml2::XMLElement& root);
        TransformComponent loadTransformComponent(tinyxml2::XMLElement& root);
        CameraComponent loadCameraComponent(tinyxml2::XMLElement& root);
        RenderComponent loadRenderComponent(tinyxml2::XMLElement& root);
        ThirdViewPlayerComponent loadThirdViewPlayerComponent(tinyxml2::XMLElement& root);
        PlayerComponent loadPlayerComponent(tinyxml2::XMLElement& root);
        LightPointComponent loadLightPointComponent(tinyxml2::XMLElement& root);
        ZombieComponent loadZombieComponent(tinyxml2::XMLElement& root);
        CameraManagerComponent loadCameraManagerComponent(tinyxml2::XMLElement& root);
        PlayerManagerComponent loadPlayerManagerComponent(tinyxml2::XMLElement& root);
        DebugPositionComponent loadDebugPositionComponent(tinyxml2::XMLElement& root);
    };
}
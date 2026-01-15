#pragma once

#include "../Component.hpp"

namespace fce
{
    enum class ScriptComponentType
    {
        THIRD_VIEW_PLAYER,
        PLAYER,
        ZOMBIE,
        CAMERA_MANAGER,
        PLAYER_MANAGER,
        DEBUG_POSITION
    };

    class ScriptComponent : public Component
    {
    public:
        ScriptComponent() = default;
        virtual ~ScriptComponent() = default;

        ScriptComponent(const ScriptComponent&) = default;
        ScriptComponent(ScriptComponent&&) = default;

        ScriptComponent& operator=(const ScriptComponent&) = default;
        ScriptComponent& operator=(ScriptComponent&&) = default;

        virtual void update() = 0;
    };

    class ThirdViewPlayerComponent;
    class PlayerComponent;
    class ZombieComponent;
    class CameraManagerComponent;
    class PlayerManagerComponent;
    class DebugPositionComponent;

    template<typename T>
    struct ScriptComponentTypeResolver;

    template<typename TYPE>
    constexpr ScriptComponentType scriptComponentTypeFrom()
    {
        return ScriptComponentTypeResolver<TYPE>::value;
    }

    template<>
    struct ScriptComponentTypeResolver<ThirdViewPlayerComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::THIRD_VIEW_PLAYER;
    };

    template<>
    struct ScriptComponentTypeResolver<PlayerComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::PLAYER;
    };

    template<>
    struct ScriptComponentTypeResolver<ZombieComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::PLAYER;
    };

    template<>
    struct ScriptComponentTypeResolver<CameraManagerComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::CAMERA_MANAGER;
    };

    template<>
    struct ScriptComponentTypeResolver<PlayerManagerComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::PLAYER_MANAGER;
    };

    template<>
    struct ScriptComponentTypeResolver<DebugPositionComponent>
    {
        static constexpr ScriptComponentType value = ScriptComponentType::DEBUG_POSITION;
    };

    template<typename TYPE>
    struct isScriptComponentResolver
    {
        static constexpr bool value = std::is_base_of<ScriptComponent, TYPE>::value;
    };

    template<typename TYPE>
    constexpr bool isScriptComponent()
    {
        return isScriptComponentResolver<TYPE>::value;
    }
}
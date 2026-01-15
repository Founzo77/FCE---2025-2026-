#pragma once

#include "../Component.hpp"

namespace fce
{
    enum class EngineComponentType
    {
        TRANSFORM,
        CAMERA,
        RENDER,
        LIGHT_POINT
    };

    class EngineComponent : public Component
    {
    public:
        EngineComponent() = default;
        virtual ~EngineComponent() = default;

        EngineComponent(const EngineComponent&) = default;
        EngineComponent(EngineComponent&&) = default;

        EngineComponent& operator=(const EngineComponent&) = default;
        EngineComponent& operator=(EngineComponent&&) = default;
    };

    class TransformComponent;
    class CameraComponent;
    class RenderComponent;
    class LightPointComponent;

    template<typename T>
    struct EngineComponentTypeResolver;

    template<typename TYPE>
    constexpr EngineComponentType engineComponentTypeFrom()
    {
        return EngineComponentTypeResolver<TYPE>::value;
    }

    template<>
    struct EngineComponentTypeResolver<TransformComponent>
    {
        static constexpr EngineComponentType value = EngineComponentType::TRANSFORM;
    };

    template<>
    struct EngineComponentTypeResolver<CameraComponent>
    {
        static constexpr EngineComponentType value = EngineComponentType::CAMERA;
    };

    template<>
    struct EngineComponentTypeResolver<RenderComponent>
    {
        static constexpr EngineComponentType value = EngineComponentType::RENDER;
    };

    template<>
    struct EngineComponentTypeResolver<LightPointComponent>
    {
        static constexpr EngineComponentType value = EngineComponentType::LIGHT_POINT;
    };

    template<typename TYPE>
    struct isEngineComponentResolver
    {
        static constexpr bool value = std::is_base_of<EngineComponent, TYPE>::value;
    };

    template<typename TYPE>
    constexpr bool isEngineComponent()
    {
        return isEngineComponentResolver<TYPE>::value;
    }
}
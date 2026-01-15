#pragma once

#include "ScriptComponent.hpp"

namespace fce
{
    struct DebugPositionComponent : public ScriptComponent
    {
    private:

    public:
        DebugPositionComponent() = default;
        virtual ~DebugPositionComponent() = default;

        DebugPositionComponent(const DebugPositionComponent&) = default;
        DebugPositionComponent(DebugPositionComponent&&) = default;

        DebugPositionComponent& operator=(const DebugPositionComponent&) = default;
        DebugPositionComponent& operator=(DebugPositionComponent&&) = default;

        void update();
    };
}
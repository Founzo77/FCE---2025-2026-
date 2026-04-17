#pragma once

#include "../GameObject.hpp"

namespace fge
{
    class Transform;
}

namespace fce
{
    class GameObjectFactory
    {
    public:
        static GameObject buildStaticRenderedGameObject(
            const fge::Transform& initialPosition, const fge::LogicalIndex meshIndex);

        static GameObject buildStaticRenderedVolumiqueGameObject(
            const fge::Transform& initialPosition, const fge::LogicalIndex volumeIndex);
    };
}

#pragma once

#include "Transform.hpp"

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    class alignas(16) Camera
    {
    public:
        Transform m_position;
        float m_physicalWidth;
        float m_physicalHeight;
        float m_depth;

    public:
        Camera() = default;
        ~Camera() = default;

        Camera(const XMFLOAT3& position, const XMFLOAT3& forward, const XMFLOAT3& up,
           float physicalWidth, float physicalHeight, float depth);

        Camera(const Camera&) = default;
        Camera& operator=(const Camera&) = default;
        Camera(Camera&&) = default;
        Camera& operator=(Camera&&) = default;

        void moveForward(float distance);
        void moveRight(float distance);
        void moveUp(float distance);

        void rotateY(float angleRad);
        void rotate(float xRad, float yRad);
    };

    inline bool operator==(const Camera& lhs, const Camera& rhs)
    {
        return lhs.m_position == rhs.m_position &&
            floatEqual(lhs.m_physicalWidth, rhs.m_physicalWidth) &&
            floatEqual(lhs.m_physicalHeight, rhs.m_physicalHeight) &&
            floatEqual(lhs.m_depth, rhs.m_depth);
    }

    inline bool operator!=(const Camera& lhs, const Camera& rhs)
    {
        return !(lhs == rhs);
    }
}
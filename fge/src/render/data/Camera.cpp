#include <fge/render/data/Camera.hpp>

namespace fge
{
    Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& forward, const XMFLOAT3& up,
        float physicalWidth, float physicalHeight, float depth) : 
        m_position(position, {forward, up}),
        m_physicalWidth(physicalHeight), m_physicalHeight(physicalHeight), m_depth(depth)
    {
        
    }

    void Camera::moveForward(float distance)
    {
        m_position.moveForward(distance);
    }

    void Camera::moveRight(float distance)
    {
        m_position.moveRight(distance);
    }

    void Camera::moveUp(float distance)
    {
        m_position.moveUp(distance);
    }

    void Camera::rotateY(float angleRad)
    {
        m_position.m_basis.rotateY(angleRad);
    }

    void Camera::rotate(float xRad, float yRad)
    {
        // Rotation "pitch" (autour de l'axe X local → haut/bas)
        m_position.m_basis.rotateAroundAxis(m_position.m_basis.right(), xRad);

        // Rotation "yaw" (autour de l'axe Y global → gauche/droite)
        XMVECTOR globalUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        m_position.m_basis.rotateAroundAxis(globalUp, yRad);
    }
}
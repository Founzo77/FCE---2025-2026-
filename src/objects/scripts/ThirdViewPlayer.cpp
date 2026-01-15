#include <fce/objects/scripts/ThirdViewPlayerComponent.hpp>

#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>

#include <fce/UpdateContext.hpp>

#include <algorithm>

namespace fce
{
    void ThirdViewPlayerComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();

        const InputManager& input = ctx.getInput();
        double dt = ctx.getDeltaTime();

        GameObject& playerGO = self.getObjectManager()->getGameObject(m_playerObjectIndex);
        TransformComponent* playerTransform = playerGO.getComponent<TransformComponent>();

        GameObject& cameraGO = self.getObjectManager()->getGameObject(m_cameraIndex);
        TransformComponent* cameraTransform = cameraGO.getComponent<TransformComponent>();

        // === PARAMÈTRES ===
        const float minPitch = -1.2f;
        const float maxPitch = 0.5f;

        if (input.getMouseButton(VK_RBUTTON) == KeyState::PUSH ||
            input.getMouseButton(VK_RBUTTON) == KeyState::PRESSED)
        {
            POINT delta = input.getDeltaMouse();
            m_yaw += delta.x * m_mouseSensitivity * dt;
            m_pitch += delta.y * m_mouseSensitivity * dt;
        }

        m_pitch = std::clamp(m_pitch, minPitch, maxPitch);

        float wheel = input.getMouseWheelDelta();
        m_distance -= wheel * 0.0001f;
        m_distance = std::clamp(m_distance, m_minDistance, m_maxDistance);

        Transform playerT = playerTransform->getTransform();

        XMVECTOR P = playerT.position();
        XMVECTOR F = playerT.forward();
        XMVECTOR R = playerT.right();
        XMVECTOR U = playerT.up();

        XMVECTOR dir = F;
        dir = XMVector3TransformNormal(dir, XMMatrixRotationAxis(R, m_pitch));
        dir = XMVector3TransformNormal(dir, XMMatrixRotationAxis(U, m_yaw));
        dir = XMVector3Normalize(dir);

        XMVECTOR camPos =
            P
            - dir * m_distance     // derrière
            + U * m_heightOffset   // surélevée
            + R * m_sideOffset;    // décalée sur la droite

        Transform camT;
        camT.setPosition(camPos);

        camT.m_basis.m_forward = XMFLOAT3(XMVectorGetX(dir), XMVectorGetY(dir), XMVectorGetZ(dir));
        camT.m_basis.m_right = playerT.m_basis.m_right;
        camT.m_basis.m_up = playerT.m_basis.m_up;
        camT.m_basis.orthonormalize();

        cameraTransform->setTransform(camT);
    }
}

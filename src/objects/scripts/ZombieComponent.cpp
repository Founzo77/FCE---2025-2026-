#include <fce/objects/scripts/ZombieComponent.hpp>

#include <fce/objects/GameObject.hpp>
#include <fce/objects/ObjectManager.hpp>
#include <fce/objects/engine_components/TransformComponent.hpp>

#include <fce/UpdateContext.hpp>

#include <fge/io/GlobalLogger.hpp>
using fge::globalLogger;

namespace fce
{
    void ZombieComponent::update()
    {
        GameObject& self = getGameObject();
        const UpdateContext& ctx = *self.getUpdateContext();
        double dt = ctx.getDeltaTime();

        TransformComponent* zombieTransform = self.getComponent<TransformComponent>();

        ObjectManager* om = self.getObjectManager();
        GameObject& targetGO = om->getGameObject(m_targetGameObjectIndex);

        TransformComponent* targetTransform = targetGO.getComponent<TransformComponent>();

        Transform zT = zombieTransform->getTransform();
        Transform tT = targetTransform->getTransform();

        XMVECTOR Zpos = zT.position();
        XMVECTOR Tpos = tT.position();

        XMVECTOR toTarget = XMVectorSubtract(Tpos, Zpos);
        float dist = XMVectorGetX(XMVector3Length(toTarget));

        if (dist < m_stopDistance)
        {
            // trop proche → ne bouge plus
            return;
        }

        XMVECTOR dir = XMVector3Normalize(toTarget);

        // direction actuelle du zombie
        XMVECTOR currentF = zT.forward();

        // interpolation angulaire pour une rotation fluide
        XMVECTOR newF = XMVectorLerp(currentF, dir, m_rotateSpeed * dt);
        newF = XMVector3Normalize(newF);

        zT.m_basis.m_forward = {
            XMVectorGetX(newF),
            XMVectorGetY(newF),
            XMVectorGetZ(newF)
        };

        zT.m_basis.orthonormalize();

        zT.moveForward(m_moveSpeed * dt);

        zombieTransform->setTransform(zT);
    }
}
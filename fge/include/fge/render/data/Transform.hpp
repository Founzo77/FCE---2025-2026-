#pragma once

#include "Basis.hpp"

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    class Transform
    {
    public:
        XMFLOAT3 m_position;
        Basis m_basis;

    public:
        Transform() noexcept
            : m_position(0.f, 0.f, 0.f), m_basis()
        {}

        Transform(const XMFLOAT3& position, const Basis& basis) noexcept
            : m_position(position), m_basis(basis)
        {}

        explicit Transform(const XMVECTOR& position, const XMMATRIX& rotation) noexcept
        {
            XMStoreFloat3(&m_position, position);
            m_basis = Basis(rotation);
        }

        Transform(const Transform&) = default;
        Transform(Transform&&) = default;

        Transform& operator=(const Transform&) = default;
        Transform& operator=(Transform&&) = default;

        ~Transform() = default;

        // === Accès vectoriels ===
        inline XMVECTOR position() const noexcept { return XMLoadFloat3(&m_position); }
        inline void setPosition(const XMVECTOR& position) noexcept 
            { XMStoreFloat3(&m_position, position); }
        inline void setPosition(const XMFLOAT3& position) noexcept 
            { m_position = position; }

        inline XMVECTOR forward() const noexcept { return m_basis.forward(); }
        inline XMVECTOR up() const noexcept { return m_basis.up(); }
        inline XMVECTOR right() const noexcept { return m_basis.right(); }

        // === Conversions en matrices ===

        /// @brief Retourne la matrice monde (rotation + translation)
        inline XMMATRIX worldMatrix() const noexcept
        {
            XMMATRIX rot = m_basis.toMatrix();
            XMVECTOR pos = XMLoadFloat3(&m_position);
            rot.r[3] = XMVectorSetW(pos, 1.f);
            return rot;
        }

        /// @brief Retourne la matrice inverse du transform (utile pour caméra/view)
        inline XMMATRIX inverseMatrix() const noexcept
        {
            XMMATRIX invRot = m_basis.inverseMatrix();
            XMVECTOR negPos = XMVectorNegate(position());
            XMVECTOR trans = XMVector3TransformCoord(negPos, invRot);
            invRot.r[3] = XMVectorSetW(trans, 1.f);
            return invRot;
        }

        /// @brief Retourne la matrice view (identique à inverseMatrix)
        inline XMMATRIX viewMatrix() const noexcept { return inverseMatrix(); }

        // === Mouvements ===

        inline void translate(const XMVECTOR& delta) noexcept
        {
            XMVECTOR pos = position();
            pos = XMVectorAdd(pos, delta);
            XMStoreFloat3(&m_position, pos);
        }

        inline void translateLocal(const XMVECTOR& localDelta) noexcept
        {
            // déplacement dans l'espace local du transform
            XMVECTOR worldDelta =
                XMVector3TransformCoord(localDelta, m_basis.toMatrix());
            translate(worldDelta);
        }

        inline void moveForward(float distance) noexcept
        {
            XMVECTOR d = XMVectorScale(forward(), distance);
            translate(d);
        }

        inline void moveRight(float distance) noexcept
        {
            XMVECTOR d = XMVectorScale(right(), distance);
            translate(d);
        }

        inline void moveUp(float distance) noexcept
        {
            XMVECTOR d = XMVectorScale(up(), distance);
            translate(d);
        }

        // === Rotations ===

        inline void lookAt(const XMVECTOR& target, const XMVECTOR& upRef = XMVectorSet(0, 1, 0, 0)) 
            noexcept
        {
            XMVECTOR dir = XMVectorSubtract(target, position());
            m_basis = Basis::lookAt(dir, upRef);
        }

        // === Combinaisons ===

        /// @brief Combine ce transform avec un autre (composition)
        inline Transform combine(const Transform& other) const noexcept
        {
            XMMATRIX combined = XMMatrixMultiply(worldMatrix(), other.worldMatrix());
            XMMATRIX rotOnly =
            {
                combined.r[0],
                combined.r[1],
                combined.r[2],
                XMVectorSet(0,0,0,1)
            };
            Basis basis(rotOnly);
            XMVECTOR pos = combined.r[3];
            return Transform(
                XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos)),
                basis
            );
        }

        /// @brief Inverse le transform (utile pour passer monde->local)
        inline Transform inverted() const noexcept
        {
            XMMATRIX inv = inverseMatrix();
            // extraire la rotation
            XMMATRIX rotOnly =
            {
                inv.r[0],
                inv.r[1],
                inv.r[2],
                XMVectorSet(0,0,0,1)
            };
            Basis basis(rotOnly);
            XMVECTOR pos = inv.r[3];
            return Transform(
                XMFLOAT3(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos)),
                basis
            );
        }
    };
}

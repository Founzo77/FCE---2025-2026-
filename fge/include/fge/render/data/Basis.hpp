#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace fge
{
    class Basis
    {
    public:
        XMFLOAT3 m_forward; // Axe Z
        XMFLOAT3 m_up; // Axe Y
        XMFLOAT3 m_right; // Axe X
        XMFLOAT3 m_scale;

    public:
        Basis() noexcept : m_forward(0.f, 0.f, 1.f), 
            m_up(0.f, 1.f, 0.f), m_right(1.f, 0.f, 0.f), m_scale(1, 1, 1)
        {}

        Basis(const XMFLOAT3& forward, const XMFLOAT3& up, const XMFLOAT3& right) noexcept
            : m_forward(forward), m_up(up), m_right(right), m_scale(1, 1, 1)
        {}

        Basis(const XMFLOAT3& forward, const XMFLOAT3& up) noexcept : m_scale(1, 1, 1)
        {
            XMVECTOR f = XMVector3Normalize(XMLoadFloat3(&forward));
            XMVECTOR u = XMVector3Normalize(XMLoadFloat3(&up));

            XMVECTOR r = XMVector3Normalize(XMVector3Cross(u, f));
            u = XMVector3Cross(f, r);

            XMStoreFloat3(&m_forward, f);
            XMStoreFloat3(&m_up, u);
            XMStoreFloat3(&m_right, r);
        }

        /// Construit un Basis à partir d'une matrice 3x3 (partie rotation)
        explicit Basis(const XMMATRIX& matrix) noexcept : m_scale(1, 1, 1)
        {
            XMStoreFloat3(&m_right, matrix.r[0]);
            XMStoreFloat3(&m_up, matrix.r[1]);
            XMStoreFloat3(&m_forward, matrix.r[2]);
        }

        Basis(const Basis&) = default;
        Basis(Basis&&) = default;

        Basis& operator=(const Basis&) = default;
        Basis& operator=(Basis&&) = default;

        ~Basis() = default;

    public:
        // -- Accès vectorisé --
        inline XMVECTOR forward() const noexcept { return XMLoadFloat3(&m_forward); }
        inline XMVECTOR up() const noexcept { return XMLoadFloat3(&m_up); }
        inline XMVECTOR right() const noexcept { return XMLoadFloat3(&m_right); }

        inline XMMATRIX toMatrix() const noexcept
        {
            XMVECTOR r = XMVectorScale(XMLoadFloat3(&m_right),   m_scale.x);
            XMVECTOR u = XMVectorScale(XMLoadFloat3(&m_up),      m_scale.y);
            XMVECTOR f = XMVectorScale(XMLoadFloat3(&m_forward), m_scale.z);

            return XMMATRIX(
                XMVectorSetW(r, 0.f),
                XMVectorSetW(u, 0.f),
                XMVectorSetW(f, 0.f),
                XMVectorSet(0.f, 0.f, 0.f, 1.f)
            );
        }

        inline void orthonormalize() noexcept
        {
            XMVECTOR f = XMVector3Normalize(forward());
            XMVECTOR u = XMVector3Normalize(up());

            // right = up × forward
            XMVECTOR r = XMVector3Normalize(XMVector3Cross(u, f));

            // up = forward × right
            u = XMVector3Cross(f, r);

            XMStoreFloat3(&m_forward, f);
            XMStoreFloat3(&m_up, u);
            XMStoreFloat3(&m_right, r);
        }

        inline void rotate(const XMVECTOR& quaternion) noexcept
        {
            XMMATRIX rot = XMMatrixRotationQuaternion(quaternion);
            XMVECTOR f = XMVector3TransformNormal(forward(), rot);
            XMVECTOR u = XMVector3TransformNormal(up(), rot);
            XMVECTOR r = XMVector3TransformNormal(right(), rot);

            XMStoreFloat3(&m_forward, f);
            XMStoreFloat3(&m_up, u);
            XMStoreFloat3(&m_right, r);
            orthonormalize();
        }

        inline void rotateAroundAxis(const XMVECTOR& axis, float radians) noexcept
        {
            XMMATRIX rot = XMMatrixRotationAxis(axis, radians);
            XMVECTOR f = XMVector3TransformNormal(forward(), rot);
            XMVECTOR u = XMVector3TransformNormal(up(), rot);
            XMVECTOR r = XMVector3TransformNormal(right(), rot);

            XMStoreFloat3(&m_forward, f);
            XMStoreFloat3(&m_up, u);
            XMStoreFloat3(&m_right, r);
            orthonormalize();
        }

        inline void rotateY(float radians) noexcept
        {
            XMMATRIX rot = XMMatrixRotationY(radians);

            XMVECTOR f = XMVector3TransformNormal(forward(), rot);
            XMVECTOR u = XMVector3TransformNormal(up(), rot);
            XMVECTOR r = XMVector3TransformNormal(right(), rot);

            XMStoreFloat3(&m_forward, f);
            XMStoreFloat3(&m_up, u);
            XMStoreFloat3(&m_right, r);
            orthonormalize();
        }

        inline void scale(XMFLOAT3 scale) noexcept
        {
            m_scale = scale;
        }

        inline XMMATRIX inverseMatrix() const noexcept
        {
            XMMATRIX m = toMatrix();
            return XMMatrixTranspose(m);
        }

        inline bool isOrthonormal(float epsilon = 1e-3f) const noexcept
        {
            XMVECTOR f = forward(), u = up(), r = right();
            float dotFU = XMVectorGetX(XMVector3Dot(f, u));
            float dotFR = XMVectorGetX(XMVector3Dot(f, r));
            float dotUR = XMVectorGetX(XMVector3Dot(u, r));

            return fabs(dotFU) < epsilon && fabs(dotFR) < epsilon && fabs(dotUR) < epsilon;
        }

        static Basis fromQuaternion(const XMVECTOR& q) noexcept
        {
            XMMATRIX rot = XMMatrixRotationQuaternion(q);
            return Basis(rot);
        }

        static Basis lookAt(const XMVECTOR& forward, 
            const XMVECTOR& upRef = XMVectorSet(0, 1, 0, 0)) noexcept
        {
            XMVECTOR f = XMVector3Normalize(forward);
            XMVECTOR r = XMVector3Normalize(XMVector3Cross(upRef, f));
            XMVECTOR u = XMVector3Cross(f, r);

            Basis basis;
            
            XMStoreFloat3(&basis.m_forward, f);
            XMStoreFloat3(&basis.m_right, r);
            XMStoreFloat3(&basis.m_up, u);
            
            return basis;
        }
    };

    inline bool floatEqual(float a, float b, float eps = 1e-5f)
    {
        return fabs(a - b) < eps;
    }

    inline bool float3Equal(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b, float eps = 1e-5f)
    {
        return floatEqual(a.x, b.x, eps) &&
            floatEqual(a.y, b.y, eps) &&
            floatEqual(a.z, b.z, eps);
    }

    inline bool operator==(const Basis& lhs, const Basis& rhs)
    {
        return float3Equal(lhs.m_forward, rhs.m_forward) &&
            float3Equal(lhs.m_up, rhs.m_up) &&
            float3Equal(lhs.m_right, rhs.m_right) &&
            float3Equal(lhs.m_scale, rhs.m_scale);
    }

    inline bool operator!=(const Basis& lhs, const Basis& rhs)
    {
        return !(lhs == rhs);
    }
}

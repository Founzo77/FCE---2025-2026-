#pragma once

#include "GeometryReference.hpp"

#include <DirectXMath.h>

#include <cstdint>

using namespace DirectX;

namespace fge
{
    struct Instance
    {
    public:
        GeometryReference m_geometryReference;
        XMMATRIX m_transform;

    public:
        Instance() = default;
        ~Instance() = default;

        Instance(const Instance&) = default;
        Instance& operator=(const Instance&) = default;
        Instance(Instance&&) = default;
        Instance& operator=(Instance&&) = default;
    };
}
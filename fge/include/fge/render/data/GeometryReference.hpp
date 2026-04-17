#pragma once

#include <cstdint>

namespace fge
{
    enum class GeometryType
    {
        TRIANGLES,
        AABB
    };

    struct GeometryReference
    {
    public:
        GeometryType m_type;
        uint32_t m_geometryIndex;

        GeometryReference() = default;
        ~GeometryReference() = default;

        GeometryReference(const GeometryReference&) = default;
        GeometryReference& operator=(const GeometryReference&) = default;
        GeometryReference(GeometryReference&&) = default;
        GeometryReference& operator=(GeometryReference&&) = default;
    };
}
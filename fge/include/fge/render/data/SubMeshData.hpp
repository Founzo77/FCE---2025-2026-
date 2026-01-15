#pragma once

#include <cstdint>

namespace fge
{
    struct SubMeshData
    {
    public:
        uint32_t m_materialIndex;

    public:
        SubMeshData() = default;
        ~SubMeshData() = default;

        SubMeshData(const SubMeshData&) = default;
        SubMeshData& operator=(const SubMeshData&) = default;
        SubMeshData(SubMeshData&&) = default;
        SubMeshData& operator=(SubMeshData&&) = default;
    };
}
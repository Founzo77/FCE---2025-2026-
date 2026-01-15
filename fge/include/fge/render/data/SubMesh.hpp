#pragma once

#include <cstdint>

namespace fge
{
    struct SubMesh
    {
    public:
        uint32_t m_startIndices;
        uint32_t m_nbIndices;
        uint32_t m_materialIndex;

        SubMesh() noexcept : m_startIndices(0), m_nbIndices(0),
            m_materialIndex(0)
        {}

        SubMesh(uint32_t startIndices, uint32_t nbIndices,
            uint32_t materialIndex) noexcept : 
            m_startIndices(startIndices), m_nbIndices(nbIndices),
            m_materialIndex(materialIndex)
        {}
        
        SubMesh(const SubMesh&) = default;
        SubMesh(SubMesh&&) = default;

        SubMesh& operator=(const SubMesh&) = default;
        SubMesh& operator=(SubMesh&&) = default;
    };
}
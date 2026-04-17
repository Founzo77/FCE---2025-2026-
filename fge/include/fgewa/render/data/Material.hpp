#pragma once

#include "Texture2D.hpp"

#include <fge/render/indices.hpp>

#include <anari/anari.h>

#include <memory>
#include <unordered_map>

using fge::LogicalIndex;
using std::shared_ptr;
using std::unordered_map;

namespace fge
{
    class Material;
}

namespace fgewa
{
    class Device;

    class Material
    {
    private:
        ANARIMaterial m_material = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Material() = default;
        ~Material();

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&& other);
        Material& operator=(Material&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Material& MaterialData,
            const unordered_map<LogicalIndex, Texture2D>& textures2D);
        void reset();
        ANARIMaterial getHandle() noexcept;
        const ANARIMaterial getHandle() const noexcept;
        ANARIMaterial* getMemoryHandle() noexcept;
    };
}
#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

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

        Material(const Material&) = default;
        Material& operator=(const Material&) = default;
        Material(Material&&) = default;
        Material& operator=(Material&&) = default;
        
        void initialize(shared_ptr<Device> device, const fge::Material& MaterialData);
        void reset();
        ANARIMaterial getHandle() noexcept;
        const ANARIMaterial getHandle() const noexcept;
        ANARIMaterial* getMemoryHandle() noexcept;
    };
}
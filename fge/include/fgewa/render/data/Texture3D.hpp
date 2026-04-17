#pragma once

#include <anari/anari.h>

#include <memory>

using std::shared_ptr;

namespace fge
{
    class Texture;
}

namespace fgewa
{
    class Device;

    class Texture3D
    {
    private:
        ANARIArray3D m_texture3DHandle = nullptr;
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_depth;
        shared_ptr<Device> m_device = nullptr;

    public:
        Texture3D() = default;
        ~Texture3D();

        Texture3D(const Texture3D&) = delete;
        Texture3D& operator=(const Texture3D&) = delete;
        Texture3D(Texture3D&& other);
        Texture3D& operator=(Texture3D&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Texture& textureData);
        void reset();
        ANARIArray3D getTexture3DHandle() noexcept;
        const ANARIArray3D getTexture3DHandle() const noexcept;
        uint32_t getWidth() const noexcept;
        uint32_t getHeight() const noexcept;
        uint32_t getDepth() const noexcept;
    };
}

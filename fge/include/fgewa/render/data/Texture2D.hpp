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

    class Texture2D
    {
    private:
        ANARIArray2D m_textureHandle = nullptr;
        ANARISampler m_samplerHandle = nullptr;
        shared_ptr<Device> m_device = nullptr;

    public:
        Texture2D() = default;
        ~Texture2D();

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
        Texture2D(Texture2D&& other);
        Texture2D& operator=(Texture2D&& other);
        
        void initialize(shared_ptr<Device> device, const fge::Texture& textureData);
        void reset();
        ANARISampler getSamplerHandle() noexcept;
        const ANARISampler getSamplerHandle() const noexcept;
        ANARIArray2D getArray2DHandle() noexcept;
        const ANARIArray2D getArray2DHandle() const noexcept;
    };
}
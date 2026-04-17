#pragma once

#include "Object.hpp"
#include "Array.hpp"

#include <helium/utility/ChangeObserverPtr.h>

namespace fgeia
{
    class Renderer : public Object
    {
    private:
        int m_pixelSamples{1};
        float m_ambientRadiance{0.8f};
        bool m_crosshairs{false};
        bool m_denoise{true};
        anari::math::float4 m_background{0.f, 0.f, 0.f, 1.f};
        helium::ChangeObserverPtr<Array2D> m_backgroundImage;

    public:
        Renderer(FgeGlobalState* globalState);
        virtual ~Renderer() override = default;

        void commitParameters() override;
        void finalize() override;
        bool isValid() const override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Renderer*, ANARI_RENDERER);

#include <fgeia/Renderer.hpp>

namespace fgeia
{
    Renderer::Renderer(FgeGlobalState* globalState) : Object(ANARI_RENDERER, globalState), m_backgroundImage(this)
    {

    }

    void Renderer::commitParameters()
    {
        m_pixelSamples = getParam<int>("pixelSamples", 1);
        m_ambientRadiance = getParam<float>("ambientRadiance", 1.f);
        m_crosshairs = getParam<bool>("crosshairs", false);
        m_denoise = getParam<bool>("denoise", true);
        m_background = getParam<anari::math::float4>("background", anari::math::float4(0, 0, 0, 1));
        m_backgroundImage = getParamObject<Array2D>("background");
    }

    void Renderer::finalize()
    {

    }

    bool Renderer::isValid() const
    {
        return true;
    }
}

FGE_ANARI_TYPEFOR_DEFINITION(fgeia::Renderer*);

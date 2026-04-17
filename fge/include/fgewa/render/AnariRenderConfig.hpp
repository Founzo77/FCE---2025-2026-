#pragma once

#include <DirectXMath.h>

#include <string>

using namespace DirectX;
using std::string;

namespace fgewa
{
    struct AnariRenderConfig
    {
    public:
        string m_libraryName;
        string m_renderAlgorithm;
        XMFLOAT3 m_backgroundColor;
        float m_ambientRadiance;

    public:
        AnariRenderConfig() = default;
        ~AnariRenderConfig() = default;

        AnariRenderConfig(const AnariRenderConfig&) = default;
        AnariRenderConfig& operator=(const AnariRenderConfig&) = default;
        AnariRenderConfig(AnariRenderConfig&& other) = default;
        AnariRenderConfig& operator=(AnariRenderConfig&& other) = default;
    };
}
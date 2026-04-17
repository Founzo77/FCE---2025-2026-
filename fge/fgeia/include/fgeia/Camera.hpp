#pragma once

#include "Object.hpp"

#include <fge/render/data/Camera.hpp>

namespace fgeia
{
    class Camera : public Object
    {
    protected:
        helium::float3 m_position;
        helium::float3 m_direction;
        helium::float3 m_up;
        helium::float4 m_imageRegion;

        fge::Camera fgeCamera;

    public:
        Camera(FgeGlobalState* globalState);
        virtual ~Camera() override = default;

        static Camera* createInstance(std::string_view subtype, FgeGlobalState* globalState);

        virtual void commitParameters() override;
    };

    class Perspective : public Camera
    {
    private:
        float m_fovy{30.f};
        float m_aspect{1.f};
        float m_focusDistance = 0.f;
        float m_apertureRadius = 0.f;

    public:
        Perspective(FgeGlobalState* globalState);

        void commitParameters() override;
        void finalize() override;
    };
}

FGE_ANARI_TYPEFOR_SPECIALIZATION(fgeia::Camera*, ANARI_CAMERA);

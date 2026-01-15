#pragma once

#include "render/FgeScene.hpp"

#include <Windows.h>

#include <memory>

using std::shared_ptr;

namespace fge
{
    class SceneDescription;

    class FgeApplication
    {
    public:
        virtual ~FgeApplication() = default;

        virtual void initializeSystem() = 0;
        virtual void initializeMainRenderer(HWND hWnd, uint32_t width, uint32_t height, 
            SceneDescription& sceneDescription) = 0;
        virtual void update() = 0;
        virtual void stopEngine() = 0;

        virtual shared_ptr<FgeScene> getScene() = 0;
        virtual void resize(const uint32_t width, const uint32_t height) = 0;
    };
}
#pragma once

#include "io/InputManager.hpp"

#include <memory>

using std::shared_ptr;

namespace fge
{
    class FgeScene;
}

using fge::FgeScene;

namespace fce
{
    class UpdateContext
    {
    private:
        double m_deltaTime;
        InputManager m_input;
        shared_ptr<fge::FgeScene> m_renderScene;

    public:
        UpdateContext() = default;
        ~UpdateContext() = default;

        void initialize(shared_ptr<fge::FgeScene> renderScene);

        inline const double getDeltaTime() const noexcept { return m_deltaTime;};
        inline const InputManager& getInput() const noexcept { return m_input;};
        inline const fge::FgeScene& getRenderScene() const { return *m_renderScene;};

        void update();
        void onKeyDown(int vk);
        void onKeyUp(int vk);
        void onMouseButtonDown(int button);
        void onMouseButtonUp(int button);
        void onRawMouseDelta(LONG dx, LONG dy);
        void onMouseWheel(short delta);
        void onLoseFocus();
        void onHasFocus();
    
    private:
        void updateInputs();
        void updateDeltaTime();
    };
}
#include <fce/UpdateContext.hpp>

#include <chrono>

namespace fce
{
    void UpdateContext::initialize(shared_ptr<FgeScene> renderScene)
    {
        m_renderScene = renderScene;
    }

    void UpdateContext::update()
    {
        updateInputs();
        updateDeltaTime();
    }

    void UpdateContext::onKeyDown(int vk)
    {
        m_input.onKeyDown(vk);
    }

    void UpdateContext::onKeyUp(int vk)
    {
        m_input.onKeyUp(vk);
    }

    void UpdateContext::onMouseButtonDown(int button)
    {
        m_input.onMouseButtonDown(button);
    }

    void UpdateContext::onMouseButtonUp(int button)
    {
        m_input.onMouseButtonUp(button);
    }

    void UpdateContext::onRawMouseDelta(LONG dx, LONG dy)
    {
        m_input.onRawMouseDelta(dx, dy);
    }

    void UpdateContext::onMouseWheel(short delta)
    {
        m_input.onMouseWheel(delta);
    }

    void UpdateContext::onLoseFocus()
    {
        m_input.onLoseFocus();
    }

    void UpdateContext::onHasFocus()
    {
        m_input.onHasFocus();
    }

    void UpdateContext::updateInputs()
    {
        m_input.update();
    }

    void UpdateContext::updateDeltaTime()
    {
        static auto previousTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> delta = currentTime - previousTime;
        m_deltaTime = delta.count();

        previousTime = currentTime;
    }
}
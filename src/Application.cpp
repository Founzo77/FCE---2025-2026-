#include <fce/Application.hpp>

#include <fce/io/XmlReader.hpp>

namespace fce
{
    void Application::initialize(HWND hWnd, 
        uint32_t width, uint32_t height, const string& pathFileScene)
    {
        // TO_DO Path file scene

        XmlReader reader(pathFileScene);

        m_renderApplication.initializeSystem();
        m_renderApplication.initializeMainRenderer(
            hWnd, width, height, reader.m_renderReader->m_sceneDescription);
        m_updateContext.initialize(m_renderApplication.getScene());
        m_objectManager.initialize(&m_updateContext, m_renderApplication.getScene(), reader);
    }

    const UpdateContext& fce::Application::getUpdateContext()
    {
        return m_updateContext;
    }

    void Application::update()
    {
        m_updateContext.update();
        m_objectManager.update();
        m_renderApplication.update();
    }

    void Application::onKeyDown(int vk)
    {
        m_updateContext.onKeyDown(vk);
    }

    void Application::onKeyUp(int vk)
    {
        m_updateContext.onKeyUp(vk);
    }

    void Application::onMouseButtonDown(int button)
    {
        m_updateContext.onMouseButtonDown(button);
    }

    void Application::onMouseButtonUp(int button)
    {
        m_updateContext.onMouseButtonUp(button);
    }

    void Application::onRawMouseDelta(LONG dx, LONG dy)
    {
        m_updateContext.onRawMouseDelta(dx, dy);
    }

    void Application::onMouseWheel(short delta)
    {
        m_updateContext.onMouseWheel(delta);
    }

    void Application::onLoseFocus()
    {
        m_updateContext.onLoseFocus();
    }

    void Application::onHasFocus()
    {
        m_updateContext.onHasFocus();
    }

    void Application::resize(const uint32_t width, const uint32_t height)
    {
        m_renderApplication.getRender().resize(width, height);
    }

    void Application::stopEngine()
    {
        m_renderApplication.stopEngine();
    }
}

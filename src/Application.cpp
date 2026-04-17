#include <fce/Application.hpp>

#include <fce/io/XmlReader.hpp>
#include <fce/io/server/HttplibRemoteController.hpp>
#include <fce/io/server/RemoteCommandQueue.hpp>

#include <fge/Application.hpp>
#include <fgewa/AnariApplication.hpp>

namespace fce
{
    void Application::initialize(HWND hWnd, 
        uint32_t width, uint32_t height, const string& pathFileScene)
    {
        // TO_DO Path file scene

        XmlReader reader(pathFileScene);

        if(reader.m_renderReader->m_sceneDescription.m_fgeApplicationType == fge::FgeApplicationType::FGE)
        {
            m_renderApplication = std::make_unique<fge::Application>();
        }
        else if(reader.m_renderReader->m_sceneDescription.m_fgeApplicationType == fge::FgeApplicationType::ANARI)
        {
            m_renderApplication = std::make_unique<fgewa::AnariApplication>();
        }

        m_renderApplication->initializeSystem();
        m_renderApplication->initializeMainRenderer(
            hWnd, width, height, reader.m_renderReader->m_sceneDescription);
        m_updateContext.initialize(m_renderApplication->getScene());
        m_remoteCommandQueue = std::make_shared<RemoteCommandQueue>();
        // TO_DO Lire la config dans un .xml ou .json du server
        m_remoteController = std::make_unique<HttplibRemoteController>("127.0.0.1", "8080", m_remoteCommandQueue);
        m_objectManager.initialize(&m_updateContext, m_renderApplication->getScene(), 
            m_remoteCommandQueue, reader);
        m_remoteController->run();
    }

    const UpdateContext& fce::Application::getUpdateContext()
    {
        return m_updateContext;
    }

    void Application::update()
    {
        m_updateContext.update();
        m_objectManager.update();
        m_renderApplication->update();
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
        m_renderApplication->resize(width, height);
    }

    void Application::stopEngine()
    {
        m_remoteController->stop();
        m_renderApplication->stopEngine();
    }
}

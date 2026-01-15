#include <fce/io/InputManager.hpp>

namespace fce
{
    void InputManager::update()
    {
        for (int i = 0; i < NB_KEYS; ++i)
            m_keyStates[i] = computeState(m_currKeys[i], m_prevKeys[i]);

        for (int i = 0; i < NB_MOUSE_BUTTONS; ++i)
            m_mouseStates[i] = computeState(m_currMouse[i], m_prevMouse[i]);

        m_prevKeys = m_currKeys;
        m_prevMouse = m_currMouse;

        m_mouseDelta = m_rawDelta;
        m_rawDelta = {0,0};

        if(m_wheelDeltaHasChanged)
            m_wheelDeltaHasChanged = false;
        else
            m_wheelDelta = 0;
    }

    void InputManager::onKeyDown(int vk)
    {
        if(m_shouldReadInput) m_currKeys[vk] = true;
    }

    void InputManager::onKeyUp(int vk)
    {
        if(m_shouldReadInput) m_currKeys[vk] = false;
    }

    void InputManager::onMouseButtonDown(int button)
    {
        if(m_shouldReadInput) m_currMouse[button] = true;
    }

    void InputManager::onMouseButtonUp(int button)
    {
        if(m_shouldReadInput) m_currMouse[button] = false;
    }

    void InputManager::onRawMouseDelta(LONG dx, LONG dy)
    {
        if(m_shouldReadInput)
        {
            m_rawDelta.x += dx;
            m_rawDelta.y += dy;
        }
    }

    void InputManager::onMouseWheel(short delta)
    {
        if(m_shouldReadInput)
        {
            m_wheelDelta = delta;
            m_wheelDeltaHasChanged = true;
        }
    }

    void InputManager::onLoseFocus()
    {
        std::fill(m_currKeys.begin(), m_currKeys.end(), false);
        std::fill(m_prevKeys.begin(), m_prevKeys.end(), false);
        std::fill(m_keyStates.begin(), m_keyStates.end(), KeyState::IDLE);

        std::fill(m_currMouse.begin(), m_currMouse.end(), false);
        std::fill(m_prevMouse.begin(), m_prevMouse.end(), false);
        std::fill(m_mouseStates.begin(), m_mouseStates.end(), KeyState::IDLE);

        m_rawDelta = {0,0};
        m_mouseDelta = {0,0};
        m_wheelDelta = 0;
        m_wheelDeltaHasChanged = false;
        m_shouldReadInput = false;
    }

    void InputManager::onHasFocus()
    {
        m_rawDelta = {0,0};
        m_mouseDelta = {0,0};
        m_wheelDelta = 0;
        m_wheelDeltaHasChanged = false;
        m_shouldReadInput = true;
    }
}
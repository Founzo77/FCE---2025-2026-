#pragma once

#include <Windows.h>
#include <cstdint>
#include <array>

namespace fce
{
    enum class KeyState : uint8_t
    {
        IDLE = 0,
        PRESSED,
        PUSH,
        RELEASED
    };

    class InputManager
    {
    public:
        static constexpr int NB_KEYS = 256;
        static constexpr int NB_MOUSE_BUTTONS = 5;

    private:
        std::array<bool, NB_KEYS> m_prevKeys{};
        std::array<bool, NB_KEYS> m_currKeys{};
        std::array<KeyState, NB_KEYS> m_keyStates{};

        std::array<bool, NB_MOUSE_BUTTONS> m_prevMouse{};
        std::array<bool, NB_MOUSE_BUTTONS> m_currMouse{};
        std::array<KeyState, NB_MOUSE_BUTTONS> m_mouseStates{};

        POINT m_mousePosition{};
        POINT m_rawDelta{0,0};
        POINT m_mouseDelta{};
        int m_wheelDelta = 0;
        bool m_wheelDeltaHasChanged = true;
        bool m_shouldReadInput = true;

    public:
        InputManager() noexcept = default;
        ~InputManager() = default;

        KeyState getKeyState(int vk) const  noexcept
            { return m_keyStates[vk]; }
        KeyState getMouseButton(int button) const  noexcept
            { return m_mouseStates[button]; }
        POINT getDeltaMouse()  const  noexcept
            { return m_mouseDelta; }
        POINT getMousePosition()  const  noexcept
            { return m_mousePosition; }
        int getMouseWheelDelta() const noexcept { return m_wheelDelta; }

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
        static inline KeyState computeState(bool curr, bool prev) noexcept
        {
            if (curr)
                return prev ? KeyState::PUSH : KeyState::PRESSED;
            else
                return prev ? KeyState::RELEASED : KeyState::IDLE;
        }
    };
}
#pragma once

#include "keyCodes.h"

namespace Monado {

    enum class CursorMode { Normal = 0, Hidden = 1, Locked = 2 };

    class Input {
    public:
        static bool IsKeyPressed(KeyCode keycode);

        static bool IsMouseButtonPressed(int button);
        static float GetMouseX();
        static float GetMouseY();
        static std::pair<float, float> GetMousePosition();

        static void SetCursorMode(CursorMode mode);
        static CursorMode GetCursorMode();
    };

} // namespace Monado

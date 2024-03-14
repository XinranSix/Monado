#pragma once

#include "monado/core/base.h"
#include "monado/core/keyCodes.h"

namespace Monado {
    class Input {
    public:
        virtual ~Input() = default;
        static bool IsKeyPressed(KeyCode key);

        static bool IsMouseButtonPressed(int button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
} // namespace Monado

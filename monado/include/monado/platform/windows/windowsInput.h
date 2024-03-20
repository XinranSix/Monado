#pragma once

#include "monado/core/input.h"

namespace Monado {

    class WindowsInput : public Input {
    protected:
        virtual bool IsKeyPressedImpl(int keycode);

        virtual bool IsMouseButtonPressedImpl(int button);
        virtual float GetMouseXImpl();
        virtual float GetMouseYImpl();
    };

} // namespace Monado

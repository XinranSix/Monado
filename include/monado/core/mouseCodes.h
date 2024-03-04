#pragma once

#include <cstdint>

namespace Monado {
    using MouseCode = uint16_t;

    namespace Mouse {
        enum : MouseCode {
            // From glfw3.h
            Button0 = 0,
            Button1 = 1,
            Button2 = 2,
            Button3 = 3,
            Button4 = 4,
            Button5 = 5,
            Button6 = 6,
            Button7 = 7,

            ButtonLast = Button7,
            ButtonLeft = Button0,
            ButtonRight = Button1,
            ButtonMiddle = Button2
        };
    }
} // namespace Monado

// ����GLFW3.h
#define MND_MOUSE_BUTTON_1         0
#define MND_MOUSE_BUTTON_2         1
#define MND_MOUSE_BUTTON_3         2
#define MND_MOUSE_BUTTON_4         3
#define MND_MOUSE_BUTTON_5         4
#define MND_MOUSE_BUTTON_6         5
#define MND_MOUSE_BUTTON_7         6
#define MND_MOUSE_BUTTON_8         7
#define MND_MOUSE_BUTTON_LAST      MND_MOUSE_BUTTON_8
#define MND_MOUSE_BUTTON_LEFT      MND_MOUSE_BUTTON_1
#define MND_MOUSE_BUTTON_RIGHT     MND_MOUSE_BUTTON_2
#define MND_MOUSE_BUTTON_MIDDLE    MND_MOUSE_BUTTON_3

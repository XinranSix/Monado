#pragma once

#include "core.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    class  Application {
    public:
        Application();
        ~Application();
        void Run();
    };

    Application *CreateApplication();
} // namespace Monado

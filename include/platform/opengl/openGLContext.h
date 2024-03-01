#pragma once

#include "monado/renderer/graphicsContext.h"
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on
namespace Monado {
    class OpenGLContext : public GraphicsContext {
    public:
        OpenGLContext(GLFWwindow *_handle) : m_Window { _handle } {}
        virtual void Init() override;
        virtual void SwapBuffer() override;

    private:
        GLFWwindow *m_Window;
    };
} // namespace Monado

#pragma once

#include "monado/renderer/graphicsContext.h"

struct GLFWwindow;

namespace Monado {

    class OpenGLContext : public GraphicsContext {
    public:
        OpenGLContext(GLFWwindow *windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow *m_WindowHandle;
    };
} // namespace Monado

#include "platform/opengl/openGLContext.h"
#include "monado/core/log.h"

namespace Monado {
    void OpenGLContext::Init() {
        glfwMakeContextCurrent(m_Window);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        MONADO_ASSERT(status, "Failed to init glad");

        MONADO_CORE_INFO("OpenGL Info:");
        MONADO_CORE_INFO("    Vendor: {0}", (const char *)glGetString(GL_VENDOR));          // 打印厂商
        MONADO_CORE_INFO("    Renderer: {0}", (const char *)glGetString(GL_RENDERER));      // 打印GPU
        MONADO_CORE_INFO("    OPENGL Version: {0}", (const char *)glGetString(GL_VERSION)); // 打印OPENGL版本

        // 检查OpenGL版本
#ifdef HZ_ENABLE_ASSERTS
        int versionMajor;
        int versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

        HAZEL_CORE_ASSERT((versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5)),
                          "Hazel requires at least OpenGL version 4.5!");
#endif
    }

    void OpenGLContext::SwapBuffer() { glfwSwapBuffers(m_Window); }
} // namespace Monado

#include "platform/openGL/openGLContext.h"
#include "monado/core/base.h"
#include "monado/core/log.h"
#include "monado/debug/instrumentor.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle) : m_WindowHandle { windowHandle } {
        MND_CORE_ASSERT(windowHandle, "Window handle is null!")
    }

    void OpenGLContext::Init() {
        MND_PROFILE_FUNCTION();

        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        MND_CORE_ASSERT(status, "Failed to initialize Glad!");

        MND_CORE_INFO("OpenGL Info:");
        MND_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
        MND_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
        MND_CORE_INFO("  Version: {0}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    MND_CORE_ASSERT((GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5)),
                        "Monado requires at least OpenGL version 4.5!");
    }

    void OpenGLContext::SwapBuffers() {
       MND_PROFILE_FUNCTION();

        glfwSwapBuffers(m_WindowHandle);
    }

} // namespace Monado

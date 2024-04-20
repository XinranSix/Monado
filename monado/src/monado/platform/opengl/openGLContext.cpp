#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "monado/platform/opengl/openGLContext.h"
#include "monado/core/log.h"

namespace Monado {

    OpenGLContext::OpenGLContext(GLFWwindow *windowHandle) : m_WindowHandle(windowHandle) {}

    OpenGLContext::~OpenGLContext() {}

    void OpenGLContext::Create() {
        MND_CORE_INFO("OpenGLContext::Create");

        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        MND_CORE_ASSERT(status, "Failed to initialize Glad!");

        MND_CORE_INFO("OpenGL Info:");
        MND_CORE_INFO("  Vendor: {0}", (const char *)glGetString(GL_VENDOR));
        MND_CORE_INFO("  Renderer: {0}", (const char *)glGetString(GL_RENDERER));
        MND_CORE_INFO("  Version: {0}", (const char *)glGetString(GL_VERSION));

#ifdef MND_ENABLE_ASSERTS
        int versionMajor;
        int versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

        MND_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5),
                        "Monado requires at least OpenGL version 4.5!");
#endif
    }

    void OpenGLContext::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }

} // namespace Monado

#include "platform/windows/windowsWindow.h"
#include "GLFW/glfw3.h"
#include "monado/core/window.h"
#include "monado/core/core.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/mouseEvent.h"
#include "monado/event/keyEvent.h"
#include "platform/opengl/openGLContext.h"
#include "monado/renderer/renderer.h"
#include <cstdint>

namespace Monado {

    static bool s_GLFWInitialized {};

    // 在WindowsWindow子类定义在Window父类声明的函数
    // FIXME: 感觉有问题
    // 分からない
    Scope<Window> Window::Create(const WindowProps &props) { return CreateScope<WindowsWindow>(props); }

    WindowsWindow::WindowsWindow(const WindowProps &props) { Init(props); }

    WindowsWindow::~WindowsWindow() {}

    void WindowsWindow::Init(const WindowProps &props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        MND_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
        if (!s_GLFWInitialized) {
            // TODO: glfwTerminate on system shutdown
            int success = glfwInit();
            MND_CORE_ASSERT(success, "Failed to intialize GLFW!"); // 是Core.h里面预处理器指令定义了HZ_CORE_ASSERT
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            s_GLFWInitialized = true;
        }

// FIXME: if debug
#ifdef MND_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#endif

        m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
        MND_ASSERT(m_Window, "Failed to create Windows Window!");

        m_Context = new OpenGLContext(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);

        Renderer::Init();

        // glfw 规定的回调函数里，不能传入m_Data的指针，所以只能通过glfw的API设置数据的指针
        // 下面函数会把 m_Window 传进去，然后又把 m_Window 作为 lambda 的参数输入进去
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.Height = height;
            data.Width = width;
            WindowResizeEvent e(height, width);
            data.EventCallback(e);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            MouseMovedEvent e((float)xPos, (float)yPos);
            data.EventCallback(e);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            WindowCloseEvent closeEvent;
            data.EventCallback(closeEvent);
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
            switch (action) {
            case GLFW_PRESS: {
                WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
                MouseButtonPressedEvent e(button);
                data.EventCallback(e);
                break;
            }
            case GLFW_RELEASE: {
                WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
                MouseButtonReleasedEvent e(button);
                data.EventCallback(e);
                break;
            }
            default: break;
            }
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent e(key, 0);
                data.EventCallback(e);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent e(key, 1);
                data.EventCallback(e);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleaseEvent e(key);
                data.EventCallback(e);
                break;
            }
            default: break;
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int s) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            KeyTypedEvent e(s);
            data.EventCallback(e);
        });
    }

    void WindowsWindow::SetVSync(bool enabled) {
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    void WindowsWindow::OnUpdate() {
        glfwPollEvents();          // 轮询事件
        glfwSwapBuffers(m_Window); // 交换缓冲
    }

    // void WindowsWindow::OnResized(int width, int height) { glViewport(0, 0, width, height); }

    void *WindowsWindow::GetNativeWindow() const { return m_Window; }

    // only close windown, not terminate glfw
    void WindowsWindow::Shutdown() {
        if (s_GLFWInitialized) {
            glfwDestroyWindow(m_Window);
        }
    }

    void WindowsWindow::OnResized(int width, int height) { glViewport(0, 0, width, height); }
} // namespace Monado

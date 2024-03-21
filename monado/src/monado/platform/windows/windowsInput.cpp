#include "monado/platform/windows/windowsWindow.h"
#include "monado/core/input.h"
#include "monado/core/application.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    bool Input::IsKeyPressed(int keycode) {
        auto &window = static_cast<WindowsWindow &>(Application::Get().GetWindow());
        auto state = glfwGetKey(static_cast<GLFWwindow *>(window.GetNativeWindow()), keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(int button) {
        auto &window = static_cast<WindowsWindow &>(Application::Get().GetWindow());

        auto state = glfwGetMouseButton(static_cast<GLFWwindow *>(window.GetNativeWindow()), button);
        return state == GLFW_PRESS;
    }

    float Input::GetMouseX() {
        auto [x, y] = GetMousePosition();
        return (float)x;
    }

    float Input::GetMouseY() {
        auto [x, y] = GetMousePosition();
        return (float)y;
    }

    std::pair<float, float> Input::GetMousePosition() {
        auto &window = static_cast<WindowsWindow &>(Application::Get().GetWindow());

        double x, y;
        glfwGetCursorPos(static_cast<GLFWwindow *>(window.GetNativeWindow()), &x, &y);
        return { (float)x, (float)y };
    }

} // namespace Monado

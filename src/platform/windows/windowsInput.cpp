#include "monado/core/input.h"
#include "monado/core/keyCodes.h"
#include "monado/core/application.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    bool Input::IsKeyPressed(const KeyCode key) {
        auto *window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPressed(int button) {
        auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePosition() {
        auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
        double xpos {}, ypos {};
        glfwGetCursorPos(window, &xpos, &ypos);
        return { (float)xpos, (float)ypos };
    }

    float Input::GetMouseX() {
        auto [x, y] = GetMousePosition();
        return (float)x;
    }

    float Input::GetMouseY() {
        auto [x, y] = GetMousePosition();
        return (float)y;
    }

} // namespace Monado

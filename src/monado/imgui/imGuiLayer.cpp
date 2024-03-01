#pragma once

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/imgui/imGuiLayer.h"
#include "imgui.h"
#include <stdio.h>
#include "monado/core/application.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

namespace Monado {

    ImGuiLayer::ImGuiLayer() {}

    ImGuiLayer::~ImGuiLayer() {}

    // ImGuiLayer的Init函数
    void ImGuiLayer::OnAttach() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        // TEMPORARY: should eventually use Hazel key codes
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        ImGui_ImplOpenGL3_Init("#version 450");
        /*
        // 这里的函数，参考了ImGui给的例子：example_glfw_opengl3的文件里的main函数
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests
        (optional, rarely used)
        // 指定ImGui的IO按键
        // TEMPORARY: 只是暂时这么写，后期需要使用Hazel自己封装的KeyCode类
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
        // ImGui为opengl做的初始设置
        ImGui_ImplOpenGL3_Init("#version 410");*/
    }

    void ImGuiLayer::OnDetach() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnImGuiRender() {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }

    void ImGuiLayer::OnEvent(Event &e) {
        // 只有鼠标在Viewport窗口上、且窗口被Focus时, Viewport窗口才可以接收到Event
        // if (!(m_ViewportFocused /* && m_ViewportHovered*/)) { // Viewport区域以外的Event会被ImGui接受
        //     e.MarkHandled();
        // }
        EventDispatcher dispatcher(e);
	//dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&ImGuiLayer::OnMouseButtonPressed, this, std::placeholders::_1));
	//dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&ImGuiLayer::OnMouseButtonReleased, this, std::placeholders::_1));
	//dispatcher.Dispatch<MouseMovedEvent>(std::bind(&ImGuiLayer::OnMouseCursorMoved, this, std::placeholders::_1));
    }

    // Begin和End之间指向OnImGuiRender里的函数
    void ImGuiLayer::Begin() {
        // 下面这一部分原本属于Update函数里，现在这部分抽出来作为了接口，可以在exe选择性的进行Render
        // Start the Dear ImGui frame

        // 这个函数应该只会执行一次具体的内容, 会在里面创建OpenGL的ShaderProgram
        ImGui_ImplOpenGL3_NewFrame();

        // 从glfw库里获取 时间、鼠标位置、窗口和Viewport大小等参数, 赋值给ImGui::IO
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void ImGuiLayer::End() {
        ImGuiIO &io = ImGui::GetIO();
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste
        // this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::SetDarkThemeColors() {
        auto &colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4 { 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4 { 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4 { 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4 { 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4 { 0.2f, 0.205f, 0.21f, 1.0f };

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4 { 0.15f, 0.1505f, 0.151f, 1.0f };
    }

  
  /*   bool ImGuiLayer::OnMouseCursorMoved(MouseMovedEvent &e) {
        ImGuiIO &io = ImGui::GetIO();
        io.MousePos = ImVec2((float)e.GetXPos(), (float)e.GetYPos());
        return true;
    }

    bool ImGuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e) {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton()] = 1;
        return true;
    }

    bool ImGuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent &e) {
        ImGuiIO &io = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton()] = 0;
        return true;
    }
 */
} // namespace Monado

#include "sandbox/sandboxApp.h"
#include <memory>

const char *vertexShaderSource = R"(
    #version 450 core

    layout (location = 0) in vec3 aPos;

    uniform mat4 u_ViewProjection;

    void main() {
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 450 core

    out vec4 FragColor;

    void main() {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";

SandboxApp::SandboxApp() {
    MONADO_ASSERT(!s_Instance, "Already Exists an application instance");

    s_Instance = this;

    m_Window = std::unique_ptr<Monado::Window>(Monado::Window::Create());
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    // m_ImGuiLayer.reset(new Monado::ImGuiLayer());
    // m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_LayerStack.PushLayer(std::shared_ptr<ExampleLayer>(new ExampleLayer()));

    // m_Window->SetVSync(true);
}

SandboxApp::~SandboxApp() {};

Monado::Application *Monado::CreateApplication() { return new SandboxApp(); }

#include "sandbox/sandboxApp.h"
#include <memory>

SandboxApp::SandboxApp() { 
    MONADO_ASSERT(!s_Instance, "Already Exists an application instance");

    s_Instance = this;

    m_Window = std::unique_ptr<Monado::Window>(Monado::Window::Create());
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    m_ImGuiLayer.reset(new Monado::ImGuiLayer());
    m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_LayerStack.PushLayer(std::shared_ptr<ExampleLayer>(new ExampleLayer()));

    // m_Window->SetVSync(true);
}

SandboxApp::~SandboxApp() {};

Monado::Application *Monado::CreateApplication() { return new SandboxApp(); }

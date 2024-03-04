#include "flappyRocket/flappyRocketApp.h"
#include "flappyRocket/gameLayer.h"
#include "monado.h"

FlappyRocketApp::FlappyRocketApp() {
    MND_ASSERT(!s_Instance, "Already Exists an application instance");
    s_Instance = this;

    m_Window = std::unique_ptr<Monado::Window>(Monado::Window::Create(Monado::WindowProps("Flappy Rocket", 1280, 720)));
    m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    m_ImGuiLayer = new Monado::ImGuiLayer();
    m_LayerStack.PushOverlay(m_ImGuiLayer);

    m_LayerStack.PushLayer(new GameLayer());

    // m_Window->SetVSync(true);
}

Monado::Application *Monado::CreateApplication() {
    FlappyRocketApp *s = new FlappyRocketApp();
    return s;
}

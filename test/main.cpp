#include "monado.h"
#include "sandbox/exampleLayer.h"


// 注意, 这里的SandboxApp只可以创建对应的Layer, 然后加入到Application的
// m_LayerStack里, 具体调用的函数在Application.cpp的Run函数里
// Run函数是不会暴露给子类去override的
class SandboxApp : public Monado::Application {
public:
    // 其实构造函数的函数定义是放在cpp文件里的, 这里方便显示放到了类声明里
    SandboxApp() {
        MONADO_ASSERT(!s_Instance, "Already Exists an application instance");

        // m_Window = std::unique_ptr<Monado::Window>(Monado::Window::Create());
        // m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        // m_ImGuiLayer = new Monado::ImGuiLayer();
        // m_LayerStack.PushOverlay(m_ImGuiLayer);

        // m_LayerStack.PushLayer(new ExampleLayer());

        // m_Window->SetVSync(true);
    }

    ~SandboxApp() {};
};

int main() {
    Monado::Log::Init();
    auto app = Monado::CreateApplication();
    app->Run();
    delete app;
}

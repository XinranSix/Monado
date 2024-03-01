#include "monado.h"

class Sandbox : public Monado::Application {
public:
    Sandbox() { /* m_LayerStack.PushLayer(new ExampleLayer()); */ };
    ~Sandbox() {};

private:
};

Monado::Application *Monado::CreateApplication() {
    // Sandbox *s = new Sandbox ();
    return new Monado::Application();
}

int main() {
    Monado::Log::Init();
    auto app = Monado::CreateApplication();
    app->Run();
    delete app;
}

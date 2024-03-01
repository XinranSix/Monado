#pragma once

#include "core.h"
#include "window.h"
#include "layerStack.h"
#include "monado/event/ApplicationEvent.h"
#include "monado/imGui/imGuiLayer.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/renderer/Shader.h"
#include "monado/renderer/Buffer.h"
#include "monado/renderer/vertexArray.h"
#include "monado/renderer/renderer.h"
#include <memory>
// #include "Timestep.h"

namespace Monado {
    // Application类是Hazel引擎提供给外部使用引擎的接口类, 这里声明了一个全局函数
    // Application* CreateApplication(); 但是没有定义它
    // 会留给使用引擎的用户来定义这个函数, 返回对应用户自定义的继承Application的类对象
    // 在main函数里, 会调用：
    // auto app = Hazel::CreateApplication();
    // app->Run();
    class Application {
    public:
        Application();
        virtual ~Application();
        inline static Application &Get() { return *s_Instance; }

        void Run(); // 注意, Run不是虚函数, 用户的自定义类无法override此函数

        void PushLayer(Layer *layer);
        Layer *PopLayer();
        Window &GetWindow() const { return *m_Window; }

        void OnEvent(Event &e); // 此函数绑定到了Window的各种事件上
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResized(WindowResizedEvent &e);

    protected:
        static Application *s_Instance;

    protected:
        std::unique_ptr<Window> m_Window;

        std::shared_ptr<ImGuiLayer> m_ImGuiLayer;
        LayerStack m_LayerStack;
        bool m_Running = true;
        bool m_Minimized = false;

    private:
        float m_LastTimestep;
        bool m_FirstFrame = true;
    };

    Application *CreateApplication();
} // namespace Monado

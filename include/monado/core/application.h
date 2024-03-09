#pragma once

#include "core.h"
#include "window.h"
#include "layer.h"
#include "layerStack.h"
#include "monado/event/applicationEvent.h"
#include "monado/imgui/imGuiLayer.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#pragma once

// #include "Base.h"

// #include "Window.h"

namespace Monado {
    struct ApplicationCommandLineArgs {
        int Count = 0;
        char **Args = nullptr;

        const char *operator[](int index) const {
            MND_CORE_ASSERT((index < Count), "下标越界");
            return Args[index];
        }
    };
    struct ApplicationSpecification {
        std::string Name = "Game Engine Application";
        std::string WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;
    };
    class Application {
    public:
        // Application(const std::string& name = "Game Engine", ApplicationCommandLineArgs args =
        // ApplicationCommandLineArgs());
        Application(const ApplicationSpecification &specification);
        virtual ~Application();

        void Run();
        void OnEvent(Event &e);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *layer);

        inline Window &GetWindow() { return *m_Window; }
        inline static Application &Get() { return *s_Instance; }

        ImGuiLayer *GetImGuiLayer() { return m_ImGuiLayer; }

        // inline OrthographicCamera& GetCamera() { return m_Camera; }
        void Close();

        const ApplicationSpecification GetSpecification() const { return m_Specification; }

    private:
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);
        Scope<Window> m_Window;
        ImGuiLayer *m_ImGuiLayer;
        bool m_Running = true;
        bool m_Minimized = false;
        LayerStack m_LayerStack;

        // ����deltatime,ÿһ֡�ļ��ʱ��
        float m_LastFrameTime = 0.0f;

    private:
        ApplicationSpecification m_Specification;
        // ApplicationCommandLineArgs m_CommandLineArgs;
        static Application *s_Instance;
    };

    // ���ڿͻ��˱�����
    Application *CreateApplication(ApplicationCommandLineArgs args);
} // namespace Monado

// namespace Monado {

//     class Application {
//     public:
//         Application();
//         virtual ~Application();

//         inline static Application &Get() { return *s_Instance; }

//         void Run();

//         void PushLayer(Layer *layer);
//         void PushOverlay(Layer *layer);
//         Layer *PopLayer();
//         Window &GetWindow() const { return *m_Window; }

//         void OnEvent(Event &e);
//         bool OnWindowClose(WindowCloseEvent &e);
//         bool OnWindowResize(WindowResizeEvent &e);

//     public:
//         // FIXME: 此处应为 protected
//         ImGuiLayer *m_ImGuiLayer;

//     protected:
//         static Application *s_Instance;
//         std::unique_ptr<Window> m_Window;
//         bool m_Running = true;
//         LayerStack m_LayerStack;
//         bool m_Minimized = false;

//     private:
//         float m_LastTimestep;
//         bool m_FirstFrame = true;
//     };

//     Application *CreateApplication();
// } // namespace Monado

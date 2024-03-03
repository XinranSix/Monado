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

namespace Monado {

    class Application {
    public:
        Application();
        virtual ~Application();

        inline static Application &Get() { return *s_Instance; }

        void Run();

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *layer);
        Layer *PopLayer();
        Window &GetWindow() const { return *m_Window; }

        void OnEvent(Event &e);
        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);

    protected:
        static Application *s_Instance;
        std::unique_ptr<Window> m_Window;
        ImGuiLayer *m_ImGuiLayer;
        bool m_Running = true;
        LayerStack m_LayerStack;
        bool m_Minimized = false;

    private:
        float m_LastTimestep;
        bool m_FirstFrame = true;
    };

    Application *CreateApplication();
} // namespace Monado

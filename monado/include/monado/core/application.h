#pragma once

#include "base.h"
#include "layerStack.h"
#include "window.h"
#include "monado/core/events/applicationEvent.h"
#include "monado/imGui/imGuiLayer.h"

namespace Monado {

    class Application {
    public:
        Application();
        virtual ~Application();

        void Run();

        virtual void OnInit() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate() {}

        virtual void OnEvent(Event &event);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *layer);
        void RenderImGui();

        std::string OpenFile(const std::string &filter) const;

        inline Window &GetWindow() { return *m_Window; }

        static inline Application &Get() { return *s_Instance; }

    private:
        bool OnWindowResize(WindowResizeEvent &e);
        bool OnWindowClose(WindowCloseEvent &e);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;
        ImGuiLayer *m_ImGuiLayer;

        static Application *s_Instance;
    };

    // Implemented by CLIENT
    Application *CreateApplication();
} // namespace Monado

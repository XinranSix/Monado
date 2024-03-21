#pragma once

#include "base.h"
#include "layerStack.h"
#include "window.h"
#include "monado/core/events/applicationEvent.h"
#include "monado/imGui/imGuiLayer.h"
#include "timeStep.h"
#include "window.h"
#include "layerStack.h"

namespace Monado {

    struct ApplicationProps {
        std::string Name;
        uint32_t WindowWidth, WindowHeight;
    };

    class Application {
    public:
        Application(const ApplicationProps &props = { "Monado Engine", 1280, 720 });
        virtual ~Application();

        void Run();

        virtual void OnInit() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate(TimeStep ts) {}

        virtual void OnEvent(Event &event);

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *layer);
        void RenderImGui();

        std::string OpenFile(const std::string &filter) const;

        inline Window &GetWindow() { return *m_Window; }

        static inline Application &Get() { return *s_Instance; }

        float GetTime() const; // TODO: This should be in "Platform"
    private:
        bool OnWindowResize(WindowResizeEvent &e);
        bool OnWindowClose(WindowCloseEvent &e);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true, m_Minimized = false;
        LayerStack m_LayerStack;
        ImGuiLayer *m_ImGuiLayer;
        TimeStep m_TimeStep;

        float m_LastFrameTime = 0.0f;

        static Application *s_Instance;
    };

    // Implemented by CLIENT
    Application *CreateApplication();
} // namespace Monado

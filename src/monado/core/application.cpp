#include <memory>

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/core/application.h"
#include "monado/core/log.h"
#include "monado/core/base.h"
#include "monado/core/timestep.h"
#include "monado/event/event.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"
#include "monado/debug/instrumentor.h"
#include "monado/renderer//renderCommand.h"
#include "monado/utils/platformUtils.h"
#include "monado/renderer/renderer.h"
#include "monado/scripting/scriptEngine.h"

#include <filesystem>

namespace Monado {

    Application *Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification &specification) : m_Specification(specification) {
        MND_PROFILE_FUNCTION();

        MND_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // Set working directory here
        if (!m_Specification.WorkingDirectory.empty())
            std::filesystem::current_path(m_Specification.WorkingDirectory);

        m_Window = Window::Create(WindowProps(m_Specification.Name));
        m_Window->SetEventCallback(MND_BIND_EVENT_FN(Application::OnEvent));

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application() {
        MND_PROFILE_FUNCTION();

        ScriptEngine::Shutdown();
        Renderer::Shutdown();
    }

    void Application::PushLayer(Layer *layer) {
        MND_PROFILE_FUNCTION();

        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer *layer) {
        MND_PROFILE_FUNCTION();

        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    void Application::Close() { m_Running = false; }

    void Application::SubmitToMainThread(const std::function<void()> &function) {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        m_MainThreadQueue.emplace_back(function);
    }

    void Application::OnEvent(Event &e) {
        MND_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(MND_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(MND_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::Run() {
        MND_PROFILE_FUNCTION();

        while (m_Running) {
            MND_PROFILE_SCOPE("RunLoop");

            float time = Time::GetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            ExecuteMainThreadQueue();

            if (!m_Minimized) {
                {
                    MND_PROFILE_SCOPE("LayerStack OnUpdate");

                    for (Layer *layer : m_LayerStack)
                        layer->OnUpdate(timestep);
                }

                m_ImGuiLayer->Begin();
                {
                    MND_PROFILE_SCOPE("LayerStack OnImGuiRender");

                    for (Layer *layer : m_LayerStack)
                        layer->OnImGuiRender();
                }
                m_ImGuiLayer->End();
            }

            m_Window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e) {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &e) {
        MND_PROFILE_FUNCTION();

        if (e.GetWidth() == 0 || e.GetHeight() == 0) {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }

    void Application::ExecuteMainThreadQueue() {
        std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

        for (auto &func : m_MainThreadQueue) {
            func();
        }

        m_MainThreadQueue.clear();
    }
} // namespace Monado

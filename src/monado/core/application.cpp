#include <memory>

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/core/application.h"
#include "monado/core/log.h"
#include "monado/core/core.h"
#include "monado/core/timestep.h"
#include "monado/event/event.h"
#include "monado/event/applicationEvent.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"
#include "monado/debug/instrumentor.h"
#include "monado/renderer//renderCommand.h"
#include "monado/utils/platformUtils.h"
#include "monado/renderer/renderer.h"

#include <filesystem>

// #include "Hazel/Scripting/ScriptEngine.h"

namespace Monado {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
    Application *Application::s_Instance = nullptr;

    /*
            ���ڴ��ڵĴ�С��1280 ��720����16 / 9 = 1.77777
            ��ô����m_Camera��left ���� -1.6,bottomΪ-0.9�Ϳ��Խ������
            ����ô�о����ˣ�����
            1280 * 0.9 = 720 * 1.6,��ôleft��-1.6��������0.9...


            :m_Camera(-2.0f, 2.0f, -2.0f, 2.0f)
            :m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
            :m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
    */
    Application::Application(const ApplicationSpecification &specification) : m_Specification(specification) {
        MND_PROFILE_FUNCTION();
        // HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // ���ù���Ŀ¼
        if (!m_Specification.WorkingDirectory.empty()) {
            std::filesystem::current_path(m_Specification.WorkingDirectory);
        }
        // ��������
        m_Window = Window::Create(WindowProps(m_Specification.Name));
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
        // m_Window->SetVSync(true);
        // m_Window->SetVSync(false);

        // ����imgui
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        // �����Ѿ�������window��imguilayer

        // ��ʼ����Ⱦ
        Renderer::Init();
        // ScriptEngine::Init();
    }
    Application::~Application() {
        MND_PROFILE_FUNCTION();

        // ScriptEngine::Shutdown();
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
    void Application::OnEvent(Event &e) {
        MND_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose)); // OnWindowClose�Ǳ���ķ���������ȡ����Ϸѭ����
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
        // HZ_CORE_TRACE("{0}", e);

        // �Ӻ���ǰ
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
            if (e.Handled)
                break;
            (*--it)->OnEvent(e);
        }
    }
    void Application::Run() {
        MND_PROFILE_FUNCTION();

        while (m_Running) {
            MND_PROFILE_SCOPE("RunLoop");
            // �ó�ÿһ֡�ļ��ʱ��
            // float time = (float)glfwGetTime(); // �Ǵ�Ӧ�ÿ�ʼ�����ܹ���ʱ��
            float time = Time::GetTime(); // �Ǵ�Ӧ�ÿ�ʼ�����ܹ���ʱ��
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized) {
                {
                    MND_PROFILE_SCOPE("LayerStack OnUpdate");
                    // ÿһ����update
                    for (Layer *layer : m_LayerStack) {
                        layer->OnUpdate(timestep);
                    }
                }
            }
            // imgui��update
            m_ImGuiLayer->Begin();
            {
                MND_PROFILE_SCOPE("LayerStack OnImGuiRender");
                for (Layer *layer : m_LayerStack)
                    layer->OnImGuiRender();
                m_ImGuiLayer->End();
            }
            // ������update
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
    void Application::Close() { m_Running = false; }
} // namespace Monado

/* namespace Monado {

    Application *Application::s_Instance {};

    Application::Application() {
        //  s_Instance = this;
        // m_Window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application() {}

    void Application::Run() {
        while (m_Running) {

            // TODO: 这里不应该用glfw的东西
            float time = (float)glfwGetTime();
            Timestep timestep;
            if (m_FirstFrame) {
                m_FirstFrame = 0.0f;
            } else {
                timestep = time - m_LastTimestep;
            }
            m_LastTimestep = time;

            if (!m_Minimized) {
                for (auto layer : m_LayerStack) {
                    layer->OnUpdate(timestep);
                }
            }

            m_ImGuiLayer->Begin();
            for (auto layer : m_LayerStack) {
                layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    void Application::OnEvent(Event &e) {
        MND_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(MND_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(MND_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
            if (e.Handled) {
                break;
            }
            (*it)->OnEvent(e);
        }
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

    bool Application::OnWindowClose(WindowCloseEvent &e) {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent &e) {
        m_Window->OnResized(e.GetWidth(), e.GetHeight());
        return true;
    }

} // namespace Monado
 */

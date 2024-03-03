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

namespace Monado {

    Application *Application::s_Instance {};

    Application::Application() {
        //  s_Instance = this;
        // m_Window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application() {}

    void Application::Run() {
        while (m_Running) {

            Monado::RenderCommand::Clear();
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
        /*    EventDispatcher dispatcher(e);
           dispatcher.Dispatch<WindowCloseEvent>(MND_BIND_EVENT_FN(Application::OnWindowClose));

           // 从后往前顺序处理事件
           for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
               (*--it)->OnEvent(e);
               if (e.Handled) // 处理完就不要传入前一个层
               {
                   break;
               }
           } */

        EventDispatcher dispatcher(e);

        // 1. 当接受窗口来的Event时, 首先判断是否是窗口关闭的事件
        // Dispatch函数只有在Event类型跟模板T匹配时, 才响应事件
        // std::bind其实是把函数和对应的参数绑定的一起
        dispatcher.Dispatch<WindowCloseEvent>(
            // std::bind第一个参数是函数指针, 第二个代表的类对象(因为是类的成员函数)
            // 第三个代表的是放到函数的第一位
            std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
        dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

        // 2. 否则才传递到layer来执行事件
        for (auto layer : m_LayerStack) {
            layer->OnEvent(e);
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

#include "monado/core/application.h"
#include "monado/event/mouseEvent.h"
#include "monado/core/log.h"
#include "monado/core/layer.h"
#include "platform/opengl/openGLBuffer.h"
#include "platform/opengl/openGLVertexArray.h"
#include "monado/renderer/renderCommand.h"
#include "monado/renderer/renderer.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <cmath>
#include <iostream>
#include <memory>

namespace Monado {

    Monado::Application *Monado::Application::s_Instance = nullptr;

    Application::Application() {
        //  s_Instance = this;
    }

    Application::~Application() {}

    void Application::Run() {
        while (m_Running) {
            {

                // MONADO_PROFILE_TIMER("Layer Stack Update")

                Monado::RenderCommand::Clear();
                // 1. 算deltatime, deltatime是引擎内部算的东西, 会在layer里提供deltatime为参数\
                 // TODO: 这里不应该用glfw的东西
                float time = (float)glfwGetTime(); // todo: 这里不应该用glfw的东西
                Timestep timestep;
                if (m_FirstFrame) {
                    m_FirstFrame = 0.0f;
                } else {
                    timestep = time - m_LastTimestep;
                }

                m_LastTimestep = time;

                if (!m_Minimized) {
                    // 2. 再执行使用引擎的用户代码的循环
                    // Application并不应该知道调用的是哪个平台的window，Window的init操作放在Window::Create里面
                    // 所以创建完window后，可以直接调用其loop开始渲染
                    for (auto layer : m_LayerStack) {
                        layer->OnUpdate(timestep);
                    }
                }
            }

            // 3. 最后调用ImGUI的循环
            // m_ImGuiLayer->Begin();
            // for (auto layer : m_LayerStack) {
            // 每一个Layer都在调用ImGuiRender函数
            // 目前有两个Layer, Sandbox定义的ExampleLayer和构造函数添加的ImGuiLayer
            // layer->OnImGuiRender();
            // }
            // m_ImGuiLayer->End();

            {
                // HAZEL_PROFILE_TIMER("Window Update")
                // 4. 每帧结束调用glSwapBuffer函数, 把画面显示到屏幕上
                m_Window->OnUpdate();
            }
        }
    }

    void Application::PushLayer(Layer *layer) { m_LayerStack.PushLayer(std::shared_ptr<Layer>(layer)); }

    Layer *Application::PopLayer() { return m_LayerStack.PopLayer().get(); }

    // 当窗口触发事件时, 会调用此函数
    void Application::OnEvent(Event &e) {
        EventDispatcher dispatcher(e);

        // 1. 当接受窗口来的Event时, 首先判断是否是窗口关闭的事件
        // Dispatch函数只有在Event类型跟模板T匹配时, 才响应事件
        // std::bind其实是把函数和对应的参数绑定的一起
        dispatcher.Dispatch<WindowCloseEvent>(
            // std::bind第一个参数是函数指针, 第二个代表的类对象(因为是类的成员函数)
            // 第三个代表的是放到函数的第一位
            std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
        dispatcher.Dispatch<WindowResizedEvent>(std::bind(&Application::OnWindowResized, this, std::placeholders::_1));

        // 2. 否则才传递到layer来执行事件
        for (auto layer : m_LayerStack) {
            layer->OnEvent(e);
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent &e) {
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResized(WindowResizedEvent &e) {
        m_Window->OnResized(e.GetWindowWidth(), e.GetWindowHeight());
        return true;
    }
} // namespace Monado

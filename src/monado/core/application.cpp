#include "monado/core/application.h"
#include "monado/event/mouseEvent.h"
#include "monado/core/log.h"
#include "monado/core/layer.h"
#include "platform/opengl/openGLBuffer.h"
#include "platform/opengl/openGLVertexArray.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include <cmath>
#include <iostream>
#include <memory>

const char *vertexShaderSource = "#version 450 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 450 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

namespace Monado {
    Application *Application::s_Instance {};

    uint32_t indices[3] = { 0, 1, 2 };

    Application::Application() {
        MONADO_ASSERT(!s_Instance, "Already Exists an application instance");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
        // m_ImGuiLayer = std::make_shared<ImGuiLayer>();
        // m_LayerStack.PushOverlay(m_ImGuiLayer);

        float vertices[] = {
            0.5f,  0.5f,  0.0f, // 右上角
            0.5f,  -0.5f, 0.0f, // 右下角
            -0.5f, -0.5f, 0.0f, // 左下角
            -0.5f, 0.5f,  0.0f  // 左上角
        };

        unsigned int indices[] = {
            // 注意索引从0开始!
            // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
            // 这样可以由下标代表顶点组合成矩形

            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
        };

        m_Shader = Shader::Create(vertexShaderSource, fragmentShaderSource);

        m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
        m_VertexArray.reset(VertexArray::Create());
        m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices)));
        m_VertexBuffer->SetBufferLayout({ { ShaderDataType::FLOAT3, "aPos" } });
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    Application::~Application() {}

    // 游戏的核心循环
    void Application::Run() {

        std::cout << "Run Application" << std::endl;
        while (m_Running) {
            // 每帧开始Clear
            glClearColor(0.7137f, 0.7333f, 0.7686f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            m_VertexBuffer->Bind();
            // m_IndexBuffer->Bind();
            m_Shader->Bind();
            glDrawElements(GL_TRIANGLES, m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

            // Application并不应该知道调用的是哪个平台的window，Window的init操作放在Window::Create里面
            // 所以创建完window后，可以直接调用其loop开始渲染
            // for (auto layer : m_LayerStack) {
            //     layer->OnUpdate({});
            // }

            // m_ImGuiLayer->Begin();
            // for (auto layer : m_LayerStack) {
            //     // 每一个Layer都在调用ImGuiRender函数
            //     // 目前有两个Layer, Sandbox定义的ExampleLayer和构造函数添加的ImGuiLayer
            //     layer->OnImGuiRender();
            // }
            // m_ImGuiLayer->End();

            // m_FirstFrame = false;

            m_Window->OnUpdate();
        }
    }

    void Application::PushLayer(std::shared_ptr<Layer> layer) { m_LayerStack.PushLayer(layer); }

    std::shared_ptr<Monado::Layer> Application::PopLayer() { return m_LayerStack.PopLayer(); }

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

        // 2. 否则才传递到layer来执行事件, 逆序遍历是为了让ImGuiLayer最先收到Event
        uint32_t layerCnt = m_LayerStack.GetLayerCnt();
        for (int i = layerCnt - 1; i >= 0; i--) {
            if (e.IsHandled())
                break;

            m_LayerStack.GetLayer((uint32_t)i)->OnEvent(e);
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

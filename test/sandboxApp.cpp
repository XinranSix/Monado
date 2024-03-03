#include "monado.h"
#include "monado/core/timestep.h"
#include "monado/renderer/orthographicCameraController.h"
#include "monado/renderer/shader.h"
#include "monado/renderer/vertexArray.h"
#include "monado/renderer/texture.h"
#include "monado/renderer/renderer2D.h"
#include "monado/renderer/renderCommand.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

class ExampleLayer : public Monado::Layer {
public:
    ExampleLayer() : Layer { "Example" }, m_Camera { Monado::OrthographicCameraController { 1028.0 / 720.0, 1 } } {}

    void OnAttach() override {

        MND_PROFILE_FUNCTION();

        m_Texture1 = Monado::Texture2D::Create("asset/textures/Hazel_Logo_Text_Light_Square.png");

        Monado::Renderer2D::Init();

        Monado::RenderCommand::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });

        // Monado::s_Data
    }

    void OnDetach() override {}

    void OnUpdate(Monado::Timestep ts) override {
        MND_PROFILE_FUNCTION();

        Monado::Renderer2D::BeginScene(m_Camera.GetCamera());
        Monado::Renderer2D::DrawRotatedQuad({ 0.0, 0.0f }, { 1.0f, 1.0f }, 0.0f, m_Texture1);
        m_Camera.OnUpdate(ts);
        Monado::Renderer2D::EndScene();
    }

    void OnEvent(Monado::Event &e) override {
        MND_TRACE("{0}", e);
        m_Camera.OnEvent(e);
    }

    virtual void OnImGuiRender() override {
        MND_PROFILE_FUNCTION();
        ImGui::Begin("Test");
        ImGui::ColorEdit4("Flat Color Picker", glm::value_ptr(m_FlatColor));

        // for (size_t i = 0; i < m_ProfileResults.size(); i++) {
        //     // auto &result = m_ProfileResults[i];
        //     // char label[50];
        //     // strcpy_s(label, result.Name);
        //     // strcat_s(label, ": %.3fms ");                  // 保留三位小数
        //     // ImGui::Text(label, result.End - result.Start); // 打印Profile条目的名字和time
        // }
        // m_ProfileResults.clear();
        m_ProfileResult = { "CameraController.OnUpdate" };
        char label[50];
        // strcpy_s(label, m_ProfileResult.Name);
        // strcat_s(label, ": %.3fms ");
        // ImGui::Text(label, (float)m_ProfileResult.End - m_ProfileResult.Start); // 打印Profile条目的名字和time
        ImGui::End();
    }

private:
    Monado::OrthographicCameraController m_Camera;
    std::shared_ptr<Monado::Shader> m_Shader;
    std::shared_ptr<Monado::Shader> m_BlueShader;

    std::shared_ptr<Monado::VertexArray> m_VertexArray;
    std::shared_ptr<Monado::VertexArray> m_QuadVertexArray;

    std::shared_ptr<Monado::Texture2D> m_Texture1;

    // std::vector<Monado::ProfileResult> m_ProfileResults;
    Monado::ProfileResult m_ProfileResult;
    glm::vec4 m_FlatColor;
};

class SandboxApp : public Monado::Application {
public:
    SandboxApp() {
        MND_ASSERT(!s_Instance, "Already Exists an application instance");

        s_Instance = this;

        m_Window = std::unique_ptr<Monado::Window>(Monado::Window::Create());
        m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

        m_ImGuiLayer = new Monado::ImGuiLayer();
        m_LayerStack.PushOverlay(m_ImGuiLayer);

        m_LayerStack.PushLayer(new ExampleLayer());
    }

    ~SandboxApp() {}
};

Monado::Application *Monado::CreateApplication() { return new SandboxApp(); }

#include "sandbox/exampleLayer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/renderer/renderCommand.h"
#include "monado/renderer/texture.h"
#include "monado/renderer/renderer2D.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include <format>

ExampleLayer::ExampleLayer() : m_Camera { Monado::OrthographicCameraController { 1028.0 / 720.0, 1 } } {}

ExampleLayer::~ExampleLayer() {}

void ExampleLayer::OnAttach() {
    m_Texture1 = Monado::Texture2D::Create("asset/texture/Hazel_Logo_Text_Light_Square.png");

    Monado::Renderer2D::Init();

    Monado::RenderCommand::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnEvent(Monado::Event &e) { m_Camera.OnEvent(e); }

void ExampleLayer::OnUpdate(const Monado::Timestep &step) {
    Monado::Renderer2D::BeginScene(m_Camera);
    Monado::Renderer2D::DrawQuad({ 0.0, 0.0f }, 0.0f, { 1.0f, 1.0f }, m_Texture1, { 1.0f, 1.0f, 1.0f, 1.0f });
    m_Camera.OnUpdate(step);
    Monado::Renderer2D::EndScene();
}

void ExampleLayer::OnImGuiRender() {
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

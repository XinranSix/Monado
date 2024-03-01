#include "sandbox/exampleLayer.h"
#include "monado/renderer/rendererAPI.h"
#include "monado/renderer/renderCommand.h"
#include "monado/renderer/texture.h"
#include "monado/renderer/renderer2D.h"

ExampleLayer::ExampleLayer() : m_Camera { Monado::OrthographicCameraController { 19. / 6., 1 } } {}

ExampleLayer::~ExampleLayer() {}

void ExampleLayer::OnAttach() {
    m_Texture1 = Monado::Texture2D::Create("asset/texture/Hazel_Logo_Text_Light_Square.png");

    Monado::Renderer2D::Init();

    Monado::RenderCommand::SetClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });
}

void ExampleLayer::OnDetach() {}

void ExampleLayer::OnEvent(Monado::Event &e) {}

void ExampleLayer::OnUpdate(const Monado::Timestep &step) {
    Monado::Renderer2D::BeginScene(m_Camera);
    Monado::Renderer2D::DrawQuad({ -0.5, 0.0f }, 0.0f, { 10.0f, 10.0f }, m_Texture1, { 1.0f, 1.0f, 1.0f, 1.0f });
}

void ExampleLayer::OnImGuiRender() {}

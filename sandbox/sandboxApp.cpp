#include "monado.h"

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "monado/renderer/framebuffer.h"

// 用以前的炸弹人的地图
/*
0不绘制
1是墙壁
2是草地
3是箱子
*/

class ExampleLayer : public Monado::Layer {
public:
    ExampleLayer()
        : Layer { "Example" }, m_Camera { Monado::OrthographicCameraController { 1600.0f / 900.0f, true } } {}

    void OnAttach() override {

        MND_PROFILE_FUNCTION();

        Monado::FramebufferSpecification framebufferSpecification;

        m_SquareTexture = Monado::Texture2D::Create("asset/textures/Checkerboard.png");
        m_SpriteSheet = Monado::Texture2D::Create("asset/textures/RPGpack_sheet_2X.png");

        // m_TextureBush = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
        // m_TextureStair = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 3 }, { 128, 128 });
        // m_TextureTree = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 4, 1 }, { 128, 128 }, { 1, 2 });

        // map数据结构
        s_TextureMap['1'] = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 10, 8 }, { 128, 128 });
        s_TextureMap['2'] = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 11 }, { 128, 128 });
        s_TextureMap['3'] = Monado::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

        Monado::Renderer2D::Init();
        Monado::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

        framebufferSpecification.Width = 1600.0f;
        framebufferSpecification.Height = 900.0f;

        //  FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth
        Monado::FramebufferAttachmentSpecification Attachments = {
            { Monado::FramebufferTextureFormat::RGBA8 },
            { Monado::FramebufferTextureFormat::Depth },
        };

        framebufferSpecification.Attachments = Attachments;

        m_Framebuffer = Monado::Framebuffer::Create(framebufferSpecification);
    }

    void OnDetach() override {}

    void OnUpdate(Monado::Timestep ts) override {
        MND_PROFILE_FUNCTION();

        m_Camera.OnUpdate(ts);
        // Monado::Renderer2D::ResetStats();
        {
            // 这里
            MND_PROFILE_SCOPE("Renderer Prep");
            // 1.绑定自定义的帧缓冲（等于解绑到渲染到默认的帧缓冲中）
            m_Framebuffer->Bind();
            Monado::RenderCommand::Clear();
            Monado::Renderer2D::BeginScene(m_Camera.GetCamera());
            Monado::Renderer2D::DrawQuad({ -1.0, 0.0f }, { 0.8f, 0.8f }, { 0.2f, 0.8f, 0.9f, 1.0f });
            Monado::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.8f }, m_FlatColor);
            Monado::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.1f }, { 1.0f, 1.0f }, m_SquareTexture, 1.0f);

            /*   Monado::Renderer2D::BeginScene(m_Camera.GetCamera());
              Monado::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.9f }, { 1.0f, 1.0f }, m_TextureBush, 1.0f);
              Monado::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.9f }, { 1.0f, 1.0f }, m_TextureStair, 1.0f);
              Monado::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.9f }, { 1.0f, 2.0f }, m_TextureTree, 1.0f); */

            // 3.解绑帧缓冲
            Monado::Renderer2D::EndScene();
            m_Framebuffer->Unbind();
        }
    }

    void OnEvent(Monado::Event &e) override {
        MND_TRACE("{0}", e);
        m_Camera.OnEvent(e);
    }

    virtual void OnImGuiRender() override {
        MND_PROFILE_FUNCTION();

        ImGui::Begin("Settings");
        auto stats = Monado::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_FlatColor));
        // 这里/
        // imgui渲染帧缓冲中的东西(附在帧缓冲上的颜色纹理缓冲)
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image((void *)textureID, ImVec2 { 1280, 720 });
        ImGui::End();
    }

private:
    Monado::OrthographicCameraController m_Camera;
    std::shared_ptr<Monado::Shader> m_Shader;
    std::shared_ptr<Monado::Shader> m_BlueShader;

    std::shared_ptr<Monado::VertexArray> m_VertexArray;
    std::shared_ptr<Monado::VertexArray> m_QuadVertexArray;

    std::shared_ptr<Monado::Texture2D> m_SquareTexture, m_SpriteSheet;
    Monado::Ref<Monado::SubTexture2D> m_TextureBush, m_TextureStair, m_TextureTree;

    // std::vector<Monado::ProfileResult> m_ProfileResults;
    Monado::ProfileResult m_ProfileResult;
    glm::vec4 m_FlatColor { 1.0f };
    std::unordered_map<char, Monado::Ref<Monado::SubTexture2D>> s_TextureMap;
    Monado::Ref<Monado::Framebuffer> m_Framebuffer;
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

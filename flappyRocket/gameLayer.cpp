#include "flappyRocket/gameLayer.h"

#include "monado/renderer/RenderCommand.h"
#include "monado/renderer/Renderer2D.h"
#include "monado/event/keyEvent.h"
#include "monado/core/Application.h"

#include <filesystem>

GameLayer::GameLayer(const std::string &name) {
    m_Level = std::make_shared<Level>();

    Monado::Renderer2D::Init();

    std::string texturePath = "asset/textures/Rocket.png";

    Player p("RocketPlayer");
    p.SetTexture(Monado::Texture2D::Create(texturePath));

    m_Level->SetPlayer(p);
}

GameLayer::~GameLayer() {}

void GameLayer::OnAttach() {
    // ImGuiIO io = ImGui::GetIO();
    // m_Font = io.Fonts->AddFontFromFileTTF("Resources/OpenSans-Regular.ttf", 120.0f);
}

void GameLayer::OnDetach() {}

void GameLayer::OnEvent(Monado::Event &e) {
    if (e.GetEventType() == Monado::EventType::KeyPressed) {
        Monado::KeyPressedEvent *ep = dynamic_cast<Monado::KeyPressedEvent *>(&e);
        if (ep->GetKeyCode() == MND_KEY_SPACE)
            m_Level->SetSpacePressed(true);
    } else if (e.GetEventType() == Monado::EventType::KeyReleased) {
        Monado::KeyReleaseEvent *er = dynamic_cast<Monado::KeyReleaseEvent *>(&e);
        if (er->GetKeyCode() == MND_KEY_SPACE)
            m_Level->SetSpacePressed(false);
    } else if (e.GetEventType() == Monado::EventType::MouseButtonPressed) {
        Monado::MouseButtonPressedEvent *er = dynamic_cast<Monado::MouseButtonPressedEvent *>(&e);
        if (er->GetMouseButton() == 0)
            OnMouseButtonPressed(*er);
    }
}

bool GameLayer::OnMouseButtonPressed(Monado::MouseButtonPressedEvent &e) {
    if (m_State == GameState::GameOver)
        m_Level->Reset();

    m_State = GameState::Play;
    return false;
}

void GameLayer::OnUpdate(Monado::Timestep ts) {
    m_Time += ts;
    if ((int)(m_Time * 10.0f) % 8 > 4)
        m_Blink = !m_Blink;

    if (m_Level->IsGameOver())
        m_State = GameState::GameOver;

    switch (m_State) {
    case GameState::Play: {
        m_Level->OnUpdate(ts);
        break;
    }
    }

    // 每帧开始Clear
    Monado::RenderCommand::Clear();
    Monado::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

    auto pos = m_Level->GetPlayer().GetPosition();
    auto angle = m_Level->GetPlayer().GetRotation();

    auto camera = m_Level->GetCameraController().GetCamera();
    auto c = camera.GetPosition();
    auto c2 = camera.GetRotation();

    // TODO: 其实Render的部分代码可以封装到Level的OnRender函数里的
    Monado::Renderer2D::BeginScene(m_Level->GetCameraController().GetCamera());
    {
        const std::vector<Column> &columns = m_Level->GetColumns();
        std::shared_ptr<Monado::Texture2D> triTex = m_Level->GetTriangleTex();

        // 绘制关卡
        // Background
        Monado::Renderer2D::DrawRotatedQuad({ pos.x, 0.0f, -0.8f }, { 10.0f, 10.0f }, 0.0f, { 0.3f, 0.3f, 0.3f, 1.0f });

        // Floor and ceiling, 在player运动的[-1, 1]范围之外绘制border即可
        Monado::Renderer2D::DrawRotatedQuad({ pos.x, 3.5f }, { 50.0f, 5.0f }, 0.0f, m_Level->GetDynamicCollor());
        Monado::Renderer2D::DrawRotatedQuad({ pos.x, -3.5f }, { 50.0f, 5.0f }, 0.0f, m_Level->GetDynamicCollor());

        for (size_t i = 0; i < columns.size(); i++) {
            // Upper triangle
            Monado::Renderer2D::DrawRotatedQuad(columns[i].topPos, columns[i].scale, 180.0f, triTex, 1.0f,
                                                m_Level->GetDynamicCollor());

            // Lower triangle
            Monado::Renderer2D::DrawRotatedQuad(columns[i].bottomPos, columns[i].scale, 0.0f, triTex, 1.0f,
                                                m_Level->GetDynamicCollor());
        }

        // 绘制Player
        Monado::Renderer2D::DrawRotatedQuad({ pos.x, pos.y, 0.1f }, { 0.2f, 0.2f }, angle,
                                            m_Level->GetPlayer().GetTexture(), 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });

        m_Level->GetPlayer().Render();

        //// 绘制碰撞点
        // for (size_t i = 0; i < m_Level->m_DebugCollisions.size(); i++)
        //{
        //	auto p = m_Level->m_DebugCollisions[i];
        //	Monado::Renderer2D::DrawQuad({ p.x, p.y, 0.1f }, angle, { 0.025f, 0.025f }, { 1.0f, 1.0f, 1.0f, 1.0f });
        // }

        //// 绘制Player的Collider
        // for (size_t i = 0; i < 4; i++)
        //{
        //	auto p = m_Level->GetPlayer().m_CurVertices[i];
        //	Monado::Renderer2D::DrawQuad({ p.x, p.y, 0.3f }, angle, { 0.025f, 0.025f }, { 1.0f, 1.0f, 1.0f, 1.0f });
        // }

        //// 绘制代表关卡的三角形的顶点
        // for (size_t i = 0; i < m_Level->m_ColumnBounds.size(); i++)
        //{
        //	auto p = m_Level->m_ColumnBounds[i];
        //	Monado::Renderer2D::DrawQuad({ p.x, p.y, 0.1f }, 0, { 0.025f, 0.025f }, { 1.0f, 1.0f, 1.0f, 1.0f });
        // }
    }
    Monado::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender() {
    switch (m_State) {
    case GameState::Play: {
        uint32_t playerScore = m_Level->GetPlayer().GetScore();
        std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
        ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());
        break;
    }
    case GameState::MainMenu: {
        auto pos = ImGui::GetWindowPos();
        auto width = Monado::Application::Get().GetWindow().GetWidth();
        auto height = Monado::Application::Get().GetWindow().GetHeight();
        pos.x += width * 0.5f - 300.0f;
        pos.y += 50.0f;
        if (m_Blink)
            ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");
        break;
    }
    case GameState::GameOver: {
        auto pos = ImGui::GetWindowPos();
        auto width = Monado::Application::Get().GetWindow().GetWidth();
        auto height = Monado::Application::Get().GetWindow().GetHeight();
        pos.x += width * 0.5f - 300.0f;
        pos.y += 50.0f;
        if (m_Blink)
            ImGui::GetForegroundDrawList()->AddText(m_Font, 120.0f, pos, 0xffffffff, "Click to Play!");

        pos.x += 200.0f;
        pos.y += 150.0f;
        uint32_t playerScore = m_Level->GetPlayer().GetScore();
        std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
        ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, pos, 0xffffffff, scoreStr.c_str());
        break;
    }
    }
}

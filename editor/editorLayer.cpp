#include "editor/editorLayer.h"
#include "entt/entity/fwd.hpp"
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"
#include "monado/core/core.h"
#include "monado/renderer/orthographicCameraController.h"
#include "monado/renderer/framebuffer.h"
#include "monado/renderer/subTexture2D.h"
#include "monado/renderer/renderCommand.h"
#include "monado/renderer/renderer2D.h"
#include "monado/renderer/texture.h"
#include "monado/debug/instrumentor.h"
#include "monado/renderer/shader.h"
#include "monado/scene/scriptableEntity.h"
#include "glm/gtc/type_ptr.hpp"
#include "monado/core/application.h"
#include "monado/core/input.h"
#include "monado/core/keyCodes.h"
#include "monado/utils/platformUtils.h"
#include "monado/core/mouseCodes.h"

#include "imgui.h"
#include "monado/math/math.h"
#include "ImGuizmo.h"
#include <memory>
#include <filesystem>

namespace Monado {

    extern const std::filesystem::path g_AssetPath;

    EditorLayer::EditorLayer() : Layer { "EditorLayer" }, m_CameraController { 1600.0f / 900.0f, true } {}
    void EditorLayer::OnAttach() {
        MND_PROFILE_FUNCTION();

        m_CheckerboardTexture = Monado::Texture2D::Create("asset/textures/Checkerboard.png");
        m_IconPlay = Monado::Texture2D::Create("asset/icons/PlayButton.png");
        m_IconStop = Monado::Texture2D::Create("asset/icons/StopButton.png");

        Monado::FramebufferSpecification fbSpec;
        fbSpec.Width = 1600.0f;
        fbSpec.Height = 900.0f;
        fbSpec.Attachments = {
            { Monado::FramebufferTextureFormat::RGBA8 },
            { Monado::FramebufferTextureFormat::RED_INTEGER },
            { Monado::FramebufferTextureFormat::Depth },
        };
        m_Framebuffer = Monado::Framebuffer::Create(fbSpec);

        m_ActiveScene = CreateRef<Scene>();

        m_EditorCamera = EditorCamera { 30.0f, 1.778f, 0.1f, 1000.0f };

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnDetach() { MND_PROFILE_FUNCTION(); }

    EditorLayer::~EditorLayer() {}

    void EditorLayer::OnUpdate(Monado::Timestep ts) {
        MND_PROFILE_FUNCTION();

        if (FramebufferSpecification spec { m_Framebuffer->GetSpecification() };
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        if (m_ViewportFocused) {
            m_CameraController.OnUpdate(ts);
        }
        m_EditorCamera.OnUpdate(ts);

        Monado::Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        // Clear out entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        switch (m_SceneState) {
        case SceneState::Edit: m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera); break;
        case SceneState::Play: m_ActiveScene->OnUpdateRuntime(ts); break;
        }

        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;

        int mouseX = (int)mx;
        int mouseY = (int)my;
        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
            int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixelData == -1 ? Entity {} : Entity { (entt::entity)pixelData, m_ActiveScene.get() };
        }

        m_Framebuffer->Unbind();
    }

    void EditorLayer::OnOverlayRender() {}

    void EditorLayer::OnImGuiRender() {
        MND_PROFILE_FUNCTION();
        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen) {
            ImGui::PopStyleVar(2);
        }

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("New", "Ctrl+N")) {
                    NewScene();
                }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    OpenScene();
                }
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                    SaveSceneAs();
                }
                if (ImGui::MenuItem("Exit")) {
                    Application::Get().Close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        // ImGui::End();

        m_SceneHierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel.OnImGuiRender();

        ImGui::Begin("Stats");

        std::string name = "None";
        if (m_HoveredEntity) {
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        }
        ImGui::Text("Hovered Entity: %s", name.c_str());

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 { 0, 0 });
        ImGui::Begin("Viewport");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2 { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1),
                     ImVec2(1, 0));

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const wchar_t *path = (const wchar_t *)payload->Data;
                OpenScene(std::filesystem::path(g_AssetPath) / path);
            }
            ImGui::EndDragDropTarget();
        }

        // Gizmos
        Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_GizmoType != -1) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            // Camera

            // Runtime camera from entity
            // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
            // const auto &camera = cameraEntity.GetComponent<CameraComponent>().camera;
            // const glm::mat4 &cameraProjection = camera.GetProjection();
            // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

            // Editor camera
            const glm::mat4 &cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

            // Entity transform
            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapValue = 0.5f;
            // Snap to 45 deagrees for retation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) {
                snapValue = 45.0f;
            }

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                                 (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr,
                                 snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing()) {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - tc.Rotation;
                tc.Translation = translation;
                tc.Rotation += deltaRotation;
                tc.Scale = scale;
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();

        UI_Toolbar();
        ImGui::End();
    }

    void EditorLayer::UI_Toolbar() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 2 });
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
        auto &colors = ImGui::GetStyle().Colors;
        const auto &buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f });
        const auto &buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { buttonActive.x, buttonActive.y, buttonActive.z, 0.5f });

        ImGui::Begin("##toolbar", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float size = ImGui::GetWindowHeight() - 4.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { size, size }, { 0, 0 }, { 1, 1 })) {
            if (m_SceneState == SceneState::Edit) {
                OnScenePlay();
            } else if (m_SceneState == SceneState::Play) {
                OnSceneStop();
            }
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();
    }

    void EditorLayer::OnEvent(Monado::Event &e) {
        MND_TRACE("{0}", e);
        m_CameraController.OnEvent(e);
        m_EditorCamera.OnEvent(e);

        EventDispatcher dispatcher { e };
        dispatcher.Dispatch<KeyPressedEvent>(MND_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(MND_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &e) {
        // if (e.GetRepeatCount() > 0) {
        //	return false;
        // }
        if (e.IsRepeat()) {
            return false;
        }
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        switch (e.GetKeyCode()) {
        // Scene ����
        case Key::N: {
            if (control) {
                NewScene();
            }
            break;
        }
        case Key::O: {
            if (control) {
                OpenScene();
            }
            break;
        }
        case Key::S: {
            if (control && shift) {
                SaveSceneAs();
            }
            if (control) {
                SaveCurScene();
            }
            break;
        }
        case Key::D:
            if (control) {
                OnDuplicateEntity();
            }
            break;

        // Gizmos
        case Key::Q: m_GizmoType = -1; break;
        case Key::W: m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
        case Key::E: m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
        case Key::R: m_GizmoType = ImGuizmo::OPERATION::SCALE; break;
        }

        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e) {
        if (e.GetMouseButton() == Mouse::ButtonLeft) {
            if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt)) {
                m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
            }
        }
        return false;
    }

    void EditorLayer::NewScene() {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    }

    void EditorLayer::OpenScene() {
        std::string filepath = FileDialogs::OpenFile("Monado Scene (*.scene)\0*.scene\0");
        if (!filepath.empty()) {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path &path) {
        if (!path.empty()) {
            m_ActiveScene = CreateRef<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SceneHierarchyPanel.SetContext(m_ActiveScene);
            SceneSerializer serializer { m_ActiveScene };
            serializer.DeSerialize(path.string());
        }
    }

    void EditorLayer::SaveSceneAs() {
        std::string filepath = FileDialogs::SaveFile("Monado Scene (*.scene)\0*.scene");
        if (!filepath.empty()) {
            SceneSerializer serializer { m_ActiveScene };
            serializer.Serialize(filepath);
        }
    }
    void EditorLayer::SaveCurScene() {
        if (!m_ActiveScene->GetCurFilePath().empty()) {
            SerializeScene(m_ActiveScene, m_ActiveScene->GetCurFilePath());
        } else {
            SaveSceneAs();
        }
    }
    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path &path) {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }
    void EditorLayer::OnScenePlay() { m_SceneState = SceneState::Play; }

    void EditorLayer::OnSceneSimulate() {}

    void EditorLayer::OnSceneStop() { m_SceneState = SceneState::Edit; }

    void EditorLayer::OnDuplicateEntity() {}
} // namespace Monado

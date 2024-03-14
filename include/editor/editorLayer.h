#pragma once

#include "glm/fwd.hpp"
#include "particleSystem.h"
#include "monado/core/layer.h"
#include "monado/event/event.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"
#include "monado/core/base.h"
#include "monado/renderer/orthographicCameraController.h"
#include "monado/renderer/framebuffer.h"
#include "monado/renderer/subTexture2D.h"
#include "monado/renderer/renderCommand.h"
#include "monado/renderer/renderer2D.h"
#include "monado/renderer/texture.h"
#include "monado/debug/instrumentor.h"
#include "monado/renderer/shader.h"
#include "glm/gtc/type_ptr.hpp"
#include "monado/scene/scene.h"
#include "monado/scene/entity.h"
#include "monado/scene/components.h"
#include "editor/panels/sceneHierarchyPanel.h"
#include "editor/panels/contentBrowserPanel.h"
#include "monado/scene/sceneSerializer.h"

namespace Monado {
    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        void OnEvent(Event &e) override;

    private:
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path &path);
        void SaveScene();
        void SaveSceneAs();

        void OnOverlayRender();

        void SerializeScene(Ref<Scene> scene, const std::filesystem::path &path);

        void OnScenePlay();
        void OnSceneStop();
        void OnSceneSimulate();

        void OnDuplicateEntity();

        // UI Panels
        void UI_Toolbar();

    private:
        Monado::OrthographicCameraController m_CameraController;

        // Temp
        Ref<VertexArray> m_SquareVA;
        Ref<Shader> m_FlatColorShader;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Scene> m_ActiveScene;
        Ref<Scene> m_EditorScene;
        std::filesystem::path m_EditorScenePath;
        Entity m_SquareEntity;
        Entity m_CameraEntity;
        Entity m_SecondCamera;

        Entity m_HoveredEntity;

        bool m_PrimaryCamera = true;

        EditorCamera m_EditorCamera;

        Ref<Texture2D> m_CheckerboardTexture;

        bool m_ViewportFocused = false, m_ViewportHovered = false;
        glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
        glm::vec2 m_ViewportBounds[2];

        glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

        int m_GizmoType = -1;

        enum class SceneState { Edit = 0, Play = 1, Simulate = 2 };
        SceneState m_SceneState = SceneState::Edit;

        bool m_ShowPhysicsColliders {};

        // Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;

        // Editor resources
        Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;
    };

} // namespace Monado

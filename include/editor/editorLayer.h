#pragma once

#include "glm/fwd.hpp"
#include "particleSystem.h"
#include "monado/core/layer.h"
#include "monado/event/event.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"
#include "monado/core/core.h"
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
#include "monado/scene/sceneSerializer.h"

namespace Monado {
    class EditorLayer : public Layer {
    public:
        EditorLayer();
        virtual ~EditorLayer();
        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event &event) override;

    private:
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

        void OnOverlayRender();

        void NewScene();
        void OpenScene();
        void OpenScene(const std::filesystem::path &path);
        void SaveSceneAs();
        void SaveCurScene();

        void SerializeScene(Ref<Scene> scene, const std::filesystem::path &path);

        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();
        void OnDuplicateEntity();

        // UI Panels
        void UI_Toolbar();

    private:
        OrthographicCameraController m_CameraController;
        Ref<Shader> m_FlatShader;
        Ref<VertexArray> m_FlatVertexArray;
        Ref<Texture2D> m_CheckerboardTexture;
        Ref<Framebuffer> m_Framebuffer;

        Ref<Scene> m_ActiveScene;

        Ref<Scene> m_EditorScene;
        Entity m_SquareEntity;
        Entity m_CameraEntity;
        Entity m_SecondCamera;

        Entity m_HoveredEntity;

        bool m_PrimaryCamera { true };

        EditorCamera m_EditorCamera;

        glm::vec2 m_ViewportSize { 0.0f, 0.0f };

        bool m_ViewportFocused {}, m_ViewportHovered {};

        SceneHierarchyPanel m_SceneHierarchyPanel;

        glm::vec4 m_FlatColor { 0.2f, 0.3f, 0.8f, 1.0f };
        glm::vec4 m_SquareColor { 0.2f, 0.3f, 0.8f, 1.0f };

        int m_GizmoType { -1 };
        glm::vec2 m_ViewportBounds[2];
    };
} // namespace Monado

#pragma once

#include "particleSystem.h"
#include "monado/core/layer.h"
#include "monado/event/event.h"
#include "monado/event/keyEvent.h"
#include "monado/event/mouseEvent.h"

// #include "Panels/SceneHierarchyPanel.h"
// #include "Panels/ContentBrowserPanel.h"
// #include "Hazel/Renderer/EditorCamera.h"

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
        void SaveCurScene(); // ���浱ǰ����

        // void SerializeScene(Ref<Scene> scene, const std::filesystem::path &path);

        void OnScenePlay();
        void OnSceneSimulate();
        void OnSceneStop();
        // ����ʵ��
        void OnDuplicateEntity();

        // UI Panels
        void UI_Toolbar();

    private:
        /*    OrthographicCameraController m_CameraController;
           Ref<Shader> m_FlatShader; // shader�� ָ��
           Ref<VertexArray> m_FlatVertexArray;
           Ref<Texture2D> m_SquareTexture; // ��������
           Ref<Framebuffer> m_Framebuffer; // ֡����
           // ��ǰ�����
           Ref<Scene> m_ActiveScene;
           // ��ǰ�༭����
           Ref<Scene> m_EditorScene;
           Entity m_SquareEntity; // ʵ�壬��scene������
           Entity m_CameraEntity; // �����ʵ��
           Entity m_SecondCamera; // �����ʵ��

           Entity m_HoveredEntity; // ��굱ǰλ�õ�ʵ��

           bool m_PrimaryCamera = true;

           EditorCamera m_EditorCamera;

           glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

           bool m_ViewportFocused = false, m_ViewportHovered = false;

           //
           SceneHierarchyPanel m_SceneHierarchyPanel;
           ContentBrowserPanel m_ContentBrowserPanel;

           // imguizmo�滭������
           int m_GizmoType = 0;

           // Ϊ�˻�ȡ���λ��-�õ����ڵı߽�
           glm::vec2 m_ViewportBounds[2]; // [0]�����Ͻǵ����꣬[1]�����½ǵ�����

           glm::vec4 m_FlatColor = { 0.2f, 0.3f, 0.8f, 1.0f };

           // ������ͣ��ť
           enum class SceneState { Edit = 0, Play = 1, Simulate = 2 };
           SceneState m_SceneState = SceneState::Edit;
           Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;

           bool m_ShowPhysicsColliders = false; */
    };
} // namespace Monado

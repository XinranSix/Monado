#pragma once

#include "OrthographicCamera.h"
#include "texture.h"
#include "subTexture2D.h"
#include "camera.h"
#include "monado/renderer/editorCamera.h"
// #include "monado/scene/Components.h"

namespace Monado {
    class Renderer2D {
    public:
        static void Init();
        static void Shutdown();

        // Caemra��û����ͼ���������贫��transform������ͼ����
        static void BeginScene(const Camera &camera, const glm::mat4 &transform);
        // EditorCamera������ֱ�ӻ�ȡͶӰ��ͼ�������Բ���Ҫtransform
        static void BeginScene(const EditorCamera &camera);
        static void BeginScene(const OrthographicCamera &camera); // TODO:REMOVE
        static void EndScene();
        static void Flush();

        // 画矩阵
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color);
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                             float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<Texture2D> &texture,
                             float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture,
                             float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const Ref<SubTexture2D> &subtexture,
                             float tilingFactor = 1.0f, const glm::vec4 &tintColor = glm::vec4(1.0f));

        // rotation is radius
        static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                    const glm::vec4 &color);
        static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                    const glm::vec4 &color);
        static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                    const Ref<Texture2D> &texture, float tilingFactor = 1.0f,
                                    const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                    const Ref<Texture2D> &texture, float tilingFactor = 1.0f,
                                    const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec2 &position, const glm::vec2 &size, float rotation,
                                    const Ref<SubTexture2D> &subtexture, float tilingFactor = 1.0f,
                                    const glm::vec4 &tintColor = glm::vec4(1.0f));
        static void DrawRotatedQuad(const glm::vec3 &position, const glm::vec2 &size, float rotation,
                                    const Ref<SubTexture2D> &subtexture, float tilingFactor = 1.0f,
                                    const glm::vec4 &tintColor = glm::vec4(1.0f));

        // Transform����position��size��SpriteRendererComponent����olor��texture
        // static void DrawSprite(const glm::mat4 &transform, SpriteRendererComponent &src, int entityID);

        // �������Ƶķ���- ����8�� ���õ���������
        // ����Ҫsize������Ϊsize������transform��
        // ��������
        static void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
        // ������
        static void DrawQuad(const glm::mat4 &transform, const Ref<Texture2D> &texture, float tilingFactor = 1.0f,
                             const glm::vec4 &tintColor = glm::vec4(1.0f), int entityID = -1);
        // ��������
        static void DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness = 1.0f,
                               float fade = 0.005f, int entityID = -1);

        // Line
        static void DrawLine(const glm::vec3 &p0, glm::vec3 &p1, const glm::vec4 &color, int entityID = -1);

        // ����һ������λ��ȷ��4�����λ�û���rect
        static void DrawRect(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color,
                             int entityID = -1);
        // ����ʵ���transformȷ������λ���ٻ���
        static void DrawRect(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);

        // ��������
        static float GetLineWidth();
        static void SetLineWidth(float width);
        // ��ǰ��Ⱦ����Ϣ
        struct Statistics {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }
        };
        static void ResetStats();
        static Statistics GetStats();

    private:
        // ��ʼ��ʼ������Ⱦ
        static void StartBatch();
        // �ڴ治��Ϊ�˷�����ȾҪ����drawcall���ƺ�����
        static void NextBatch(); // 1.��drawcall��Ⱦ��ǰ 2.�����ڴ�ָ��
    };
} // namespace Monado

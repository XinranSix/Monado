#pragma once

#include "renderCommand.h"
#include "orthographicCamera.h"
#include "shader.h"

namespace Monado {

    class Renderer {
    public:
        // ´ÓÉãÏñ»ú¿ªÊ¼
        static void Init();
        static void Shutdown();
        static void OnWindowResize(uint32_t width, uint32_t height);
        static void BeginScene(OrthographicCamera &camera);
        static void EndScene();
        static void Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray,
                           glm::mat4 transform = glm::mat4(1.0f));
        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

    private:
        struct SceneData {
            glm::mat4 ViewProjectionMatrix;
        };
        static SceneData *m_SceneData;
    };
} // namespace Monado

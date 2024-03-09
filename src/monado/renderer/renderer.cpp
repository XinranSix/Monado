#include "monado/renderer/renderer.h"
#include "monado/renderer/renderer2D.h"
#include "monado/renderer/renderCommand.h"
#include "platform/opengl/openGLShader.h"
// #include "Renderer3D.h"
namespace Monado {

    Renderer::SceneData *Renderer::m_SceneData = new Renderer::SceneData;

    void Renderer::Init() {
        RenderCommand::Init();
        Renderer2D::Init();
        // Renderer3D::Init();
    }
    void Renderer::Shutdown() { Renderer2D::Shutdown(); }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height) { RenderCommand::SetViewport(0, 0, width, height); }

    void Renderer::BeginScene(OrthographicCamera &camera) {
        m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }
    void Renderer::EndScene() {}
    void Renderer::Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, glm::mat4 transform) {
        vertexArray->Bind(); // 绑定顶点数组

        shader->Bind(); // 绑定shader
        // 上传矩阵数据到glsl
        std::dynamic_pointer_cast<Monado::OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection",
                                                                                   m_SceneData->ViewProjectionMatrix);

        std::dynamic_pointer_cast<Monado::OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

        RenderCommand::DrawIndexed(vertexArray);
    }
} // namespace Monado

#include "platform/opengl/openGLRendererAPI.h"
#include "monado/debug/instrumentor.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {
    void OpenGLMessageCallback(unsigned source, unsigned type, unsigned id, unsigned severity, int length,
                               const char *message, const void *userParam) {
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: MND_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM: MND_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW: MND_CORE_WARN(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: MND_CORE_TRACE(message); return;
        }

        MND_CORE_ASSERT(false, "Unknown severity level!");
    }
    void OpenGLRendererAPI::Init() {
        MND_PROFILE_FUNCTION();

#ifdef MND_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
        // �������
        glEnable(GL_BLEND);
        // ��Ϻ���
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // ������Ȳ���
        glEnable(GL_DEPTH_TEST);
        // ���������⻬
        glEnable(GL_LINE_SMOOTH);
    }
    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
        glViewport(x, y, width, height);
    }
    void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color) { glClearColor(color.r, color.g, color.b, color.a); }
    void OpenGLRendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
    void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t indexCount) {
        vertexArray->Bind();
        uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
    void OpenGLRendererAPI::DrawLines(const Ref<VertexArray> &vertexArray, uint32_t vertexCount) {
        vertexArray->Bind();
        glDrawArrays(GL_LINES, 0, vertexCount);
    }
    void OpenGLRendererAPI::SetLineWidth(float width) { glLineWidth(width); }
} // namespace Monado

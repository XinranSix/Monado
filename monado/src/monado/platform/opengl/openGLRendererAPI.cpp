#include "monado/renderer/rendererAPI.h"

// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace Monado {

    void RendererAPI::Init() {
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    void RendererAPI::Shutdown() {}

    void RendererAPI::Clear(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RendererAPI::SetClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }

    void RendererAPI::DrawIndexed(unsigned int count) { glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr); }

} // namespace Monado

#include "monado/renderer/vertexArray.h"
#include "platform/openGL/openGLVertexArray.h"
#include "monado/renderer/renderer.h"
// clang-format off
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "monado/core/core.h"
#include "monado/core/log.h"

namespace Monado {
    VertexArray *VertexArray::Create() {
        VertexArray *buffer {};
        switch (Renderer::GetAPI()) {
        case RendererAPI::APIType::None: {
            MONADO_CORE_ERROR("No RendererAPI selected");
            MONADO_ASSERT(false, "Error, please choose a Renderer API");
        }
        case RendererAPI::APIType::OpenGL: {
            buffer = (new OpenGLVertexArray());
        }
        }

        return buffer;
    }
} // namespace Monado

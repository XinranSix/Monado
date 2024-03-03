#include "monado/renderer/vertexArray.h"

#include "monado/renderer/renderer.h"
#include "platform/opengl/openGLVertexArray.h"

namespace Monado {

    Ref<VertexArray> VertexArray::Create() {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
        }
        MND_CORE_ASSERT(false, "UnKnown RendererAPI!");
        return nullptr;
    }
} // namespace Monado

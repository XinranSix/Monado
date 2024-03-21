#include "monado/platform/opengl/openGLVertexArray.h"
#include "monado/renderer/renderer.h"
#include "monado/renderer/vertexArray.h"
#include "monado/core/log.h"

namespace Monado {

    Ref<VertexArray> VertexArray::Create() {
        switch (RendererAPI::Current()) {
        case RendererAPIType::None:
            MND_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPIType::OpenGL: return std::make_shared<OpenGLVertexArray>();
        }

        MND_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

} // namespace Monado

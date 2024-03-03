#include "monado/renderer/uniformBuffer.h"

#include "monado/renderer/renderer.h"
#include "platform/opengl/openGLUniformBuffer.h"

namespace Monado {
    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
        switch (Renderer::GetAPI()) {
        case RendererAPI::API::None:
            MND_CORE_ASSERT(false, "RendererAPI:None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
        }
        MND_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
} // namespace Monado
